#pragma once
#include <vdl/Fwd.hpp>

#include <vdl/pch/DirectX12/pch.hpp>

#include <vector>
#include <assert.h>

struct DescriptorHeap;

class DescriptorAllocator
{
  static constexpr vdl::uint kDescriptorHeapMultipleNum = 25;
private:
  struct FreeSpace
  {
    vdl::uint Index;
    vdl::uint Size;
  public:
    [[nodiscard]] bool operator<(const FreeSpace& _FreeSpace)const noexcept { return Index < _FreeSpace.Index; }

    [[nodiscard]] bool operator>(const FreeSpace& _FreeSpace)const noexcept { return Index > _FreeSpace.Index; }

    [[nodiscard]] bool operator<=(const FreeSpace& _FreeSpace)const noexcept { return Index <= _FreeSpace.Index; }

    [[nodiscard]] bool operator>=(const FreeSpace& _FreeSpace)const noexcept { return Index >= _FreeSpace.Index; }
  };
private:
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pDescriptorHeap_;
  D3D12_CPU_DESCRIPTOR_HANDLE BaseCPUHandle_;
  D3D12_GPU_DESCRIPTOR_HANDLE BaseGPUHandle_;
  vdl::uint DescriptorSize_;
  vdl::uint DescriptorIncrementSize_;
  std::vector<FreeSpace> FreeSpaceDatas_;
public:
  DescriptorAllocator() = default;

  ~DescriptorAllocator() = default;

  [[nodiscard]] ID3D12DescriptorHeap* GetDescriptorHeap() { return pDescriptorHeap_.Get(); }

  void Initialize(ID3D12Device* _pDevice, D3D12_DESCRIPTOR_HEAP_TYPE _Type, D3D12_DESCRIPTOR_HEAP_FLAGS _Flag = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

  void Allocate(DescriptorHeap* _Heap, vdl::uint _Size = 1);

  void Release(const D3D12_CPU_DESCRIPTOR_HANDLE& _CPUHandle, vdl::uint _Size);
};

struct DescriptorHeap
{
  DescriptorAllocator* Allocator = nullptr;
  vdl::uint Size;
  D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle;
  D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle;
public:
  DescriptorHeap() = default;

  DescriptorHeap(DescriptorHeap&& _Heap)noexcept
  {
    Allocator = _Heap.Allocator;
    _Heap.Allocator = nullptr;
    Size = _Heap.Size;
    _Heap.Size = 0;
    CPUHandle = _Heap.CPUHandle;
    GPUHandle = _Heap.GPUHandle;
  }

  ~DescriptorHeap()
  {
    if (Allocator)
    {
      Allocator->Release(CPUHandle, Size);
    }
  }

  DescriptorHeap& operator=(DescriptorHeap&& _Heap)noexcept
  {
    if (Allocator)
    {
      Allocator->Release(CPUHandle, Size);
    }

    Allocator = _Heap.Allocator;
    _Heap.Allocator = nullptr;
    Size = _Heap.Size;
    _Heap.Size = 0;
    CPUHandle = _Heap.CPUHandle;
    GPUHandle = _Heap.GPUHandle;

    return *this;
  }

  [[nodiscard]] bool operator<(const DescriptorHeap& _Heap)const noexcept { return CPUHandle.ptr < _Heap.CPUHandle.ptr; }

  [[nodiscard]] bool operator>(const DescriptorHeap& _Heap)const noexcept { return CPUHandle.ptr > _Heap.CPUHandle.ptr; }

  [[nodiscard]] bool operator<=(const DescriptorHeap& _Heap)const noexcept { return CPUHandle.ptr <= _Heap.CPUHandle.ptr; }

  [[nodiscard]] bool operator>=(const DescriptorHeap& _Heap)const noexcept { return CPUHandle.ptr >= _Heap.CPUHandle.ptr; }
};

enum class DescriptorHeapType
{
  eCBV_SRV_UAV,
  eSampler,
  eRTV,
  eDSV,

  eNum
};

inline constexpr D3D12_DESCRIPTOR_HEAP_TYPE Cast(DescriptorHeapType _Type)
{
  switch (_Type)
  {
  case DescriptorHeapType::eCBV_SRV_UAV:
    return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
  case DescriptorHeapType::eSampler:
    return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
  case DescriptorHeapType::eRTV:
    return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  case DescriptorHeapType::eDSV:
    return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
  default: assert(false);
  }

  return D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
}
