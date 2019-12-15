#include "DescriptorHeap.hpp"

#include <vdl/Constants/Constants.hpp>

#include <vdl/Misc/Windows/Misc.hpp>

namespace
{
  inline constexpr vdl::uint GetDescriptorNum(DescriptorHeapType _Type)
  {
    switch (_Type)
    {
    case DescriptorHeapType::eCBV_SRV_UAV:
      return (Constants::kMaxShaderResourceNum + Constants::kMaxConstantBufferNum + Constants::kMaxUnorderedAccessObjectNum) * kShaderStageNum;
    case DescriptorHeapType::eSampler:
      return (Constants::kMaxSamplerNum) * kShaderStageNum;
    default: assert(false);
    }

    return 0;
  }
}

void DescriptorHeap::Initialize(ID3D12Device* _pDevice, DescriptorHeapType _Type)
{
  HRESULT hr = S_OK;

  DescriptorSize_ = GetDescriptorNum(_Type) * kDescriptorMultipleNum;

  D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
  {
    DescriptorHeapDesc.Type = Cast(_Type);
    DescriptorHeapDesc.NumDescriptors = DescriptorSize_;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    DescriptorHeapDesc.NodeMask = 0;
  }

  hr = _pDevice->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(pDescriptorHeap_.GetAddressOf()));
  ERROR_CHECK(hr);

  DescriptorIncrementSize_ = _pDevice->GetDescriptorHandleIncrementSize(DescriptorHeapDesc.Type);
}

vdl::uint DescriptorHeap::Secure(vdl::uint _DescriptorSize)
{
  if (DescriptorSize_ < Offset_ + _DescriptorSize)
  {
    Offset_ = 0;
  }

  const vdl::uint Offset = Offset_;
  Offset_ += _DescriptorSize;
  return Offset;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUDescriptorHandle(vdl::uint _Offset)
{
  D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = pDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
  DescriptorHandle.ptr += _Offset * DescriptorIncrementSize_;

  return DescriptorHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUDescriptorHandle(vdl::uint _Offset)
{
  D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHandle = pDescriptorHeap_->GetGPUDescriptorHandleForHeapStart();
  DescriptorHandle.ptr += _Offset * DescriptorIncrementSize_;

  return DescriptorHandle;
}
