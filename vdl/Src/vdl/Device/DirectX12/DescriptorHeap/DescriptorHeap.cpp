#include "DescriptorHeap.hpp"

#include <vdl/Shader/DirectX12/CShader.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/Constants/Constants.hpp>

#include <assert.h>

namespace
{
  inline constexpr vdl::uint GetDescriptorNum(D3D12_DESCRIPTOR_HEAP_TYPE _Type)
  {
    switch (_Type)
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
      return (Constants::kMaxShaderResourceNum + Constants::kMaxConstantBufferNum + Constants::kMaxUnorderedAccessObjectNum) * kShaderStageNum;
    case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
      return (Constants::kMaxSamplerNum) * kShaderStageNum;
    case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
      return Constants::kMaxRenderTextureNum;
    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
      return 1;
    default: assert(false);
    }

    return 0;
  }
}

void DescriptorAllocator::Initialize(ID3D12Device* _pDevice, D3D12_DESCRIPTOR_HEAP_TYPE _Type, D3D12_DESCRIPTOR_HEAP_FLAGS _Flag)
{
  HRESULT hr = S_OK;

  D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
  {
    DescriptorHeapDesc.Type = _Type;
    DescriptorHeapDesc.NumDescriptors = DescriptorSize_ = GetDescriptorNum(_Type) * kDescriptorHeapMultipleNum;
    DescriptorHeapDesc.Flags = _Flag;
    DescriptorHeapDesc.NodeMask = 0;
  }
  hr = _pDevice->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(pDescriptorHeap_.GetAddressOf()));
  ERROR_CHECK(hr);

  BaseCPUHandle_ = pDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
  BaseGPUHandle_ = pDescriptorHeap_->GetGPUDescriptorHandleForHeapStart();

  DescriptorIncrementSize_ = _pDevice->GetDescriptorHandleIncrementSize(DescriptorHeapDesc.Type);

  FreeSpace& FreeSpace = FreeSpaceDatas_.emplace_back();
  {
    FreeSpace.Index = 0;
    FreeSpace.Size = DescriptorSize_;
  }
}

void DescriptorAllocator::Allocate(DescriptorHeap* _Heap, vdl::uint _Size)
{
  assert(!FreeSpaceDatas_.empty());
  assert(_Size > 0);

  //  �z��̌�납��f�[�^������
  //  (���ɂ�����̂̕����L���b�V�����c���Ă���\������������)
  for (auto Itr = FreeSpaceDatas_.end() - 1, Start = FreeSpaceDatas_.begin(); Start <= Itr; --Itr)
  {
    if (Itr->Size >= _Size)
    {
      const vdl::uint Index = Itr->Index + (Itr->Size -= _Size);

      _Heap->Allocator = this;
      _Heap->Size = _Size;
      _Heap->CPUHandle = BaseCPUHandle_;
      _Heap->CPUHandle.ptr += DescriptorIncrementSize_ * Index;
      _Heap->GPUHandle = BaseGPUHandle_;
      _Heap->GPUHandle.ptr += DescriptorIncrementSize_ * Index;

      //  �e�ʂ������Ȃ������ɍ폜
      if (Itr->Size == 0)
      {
        FreeSpaceDatas_.erase(Itr);
      }

      return;
    }
  }

  assert(false);
}

void DescriptorAllocator::Release(const D3D12_CPU_DESCRIPTOR_HANDLE& _CPUHandle, vdl::uint _Size)
{
  assert(_Size > 0);

  const vdl::uint Index = static_cast<vdl::uint>((_CPUHandle.ptr - BaseCPUHandle_.ptr) / DescriptorIncrementSize_);

  //  �}�[�W�\�Ȃ��̂�����
  auto Marge = [&]()->bool
  {
    for (auto& FreeSpace : FreeSpaceDatas_)
    {
      if ((FreeSpace.Index + FreeSpace.Size) == Index)
      {
        FreeSpace.Size += _Size;
        return true;
      }
    }

    return false;
  };

  if (Marge())
  {
    //  �C���f�b�N�X���Ƀ\�[�g
    std::sort(FreeSpaceDatas_.begin(), FreeSpaceDatas_.end());

    for (auto Itr = FreeSpaceDatas_.begin() + 1; Itr < FreeSpaceDatas_.end();)
    {
      auto PreviousItr = Itr - 1;
      assert(PreviousItr->Index + PreviousItr->Size <= Itr->Index);

      if (PreviousItr->Index + PreviousItr->Size == Itr->Index)
      {
        PreviousItr->Size += Itr->Size;
        Itr = FreeSpaceDatas_.erase(Itr);
      }
      else
      {
        ++Itr;
      }
    }
  }
  else
  {
    //  �}�[�W�\�łȂ��ꍇ�ɑ}��
    FreeSpace& FreeSpace = FreeSpaceDatas_.emplace_back();
    {
      FreeSpace.Index = Index;
      FreeSpace.Size = _Size;
    }
  }
}
