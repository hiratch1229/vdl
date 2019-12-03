#pragma once
#include <vdl/pch/DirectX12/pch.hpp>

#include <vdl/Shader/DirectX12/CShader.hpp>

enum class DescriptorHeapType
{
  eCBV_SRV_UAV,
  eSampler,
  eNum,
};
constexpr vdl::uint kDescriptorHeapTypeNum = static_cast<vdl::uint>(DescriptorHeapType::eNum);

class DescriptorHeap
{
  static constexpr vdl::uint kDescriptorMultipleNum = 25;
private:
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pDescriptorHeap_;
  vdl::uint DescriptorSize_;
  vdl::uint DescriptorIncrementSize_;
  vdl::uint Offset_ = 0;
public:
  [[nodiscard]] ID3D12DescriptorHeap* GetDescriptorHeap() { return pDescriptorHeap_.Get(); }
  [[nodiscard]] vdl::uint GetDescriptorIncrementSize()const { return DescriptorIncrementSize_; }
public:
  DescriptorHeap() = default;

  void Initialize(ID3D12Device* _pDevice, DescriptorHeapType _Type);

  vdl::uint Secure(vdl::uint _DescriptorSize);

  D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(vdl::uint _Offset);

  D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(vdl::uint _Offset);
};

inline constexpr D3D12_DESCRIPTOR_HEAP_TYPE Cast(DescriptorHeapType _Type)
{
  switch (_Type)
  {
  case DescriptorHeapType::eCBV_SRV_UAV:
    return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
  case DescriptorHeapType::eSampler:
    return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
  default: assert(false);
  }

  return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
}
