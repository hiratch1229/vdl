#pragma once
#include <vdl/Fwd.hpp>

#include <vdl/pch/DirectX12/pch.hpp>

class Fence
{
  Microsoft::WRL::ComPtr<ID3D12Fence> pFence_;
  vdl::uint64_t Value_;
public:
  [[nodiscard]] ID3D12Fence* Get() { return pFence_.Get(); }
  [[nodiscard]] vdl::uint64_t GetCurrentValue()const { return Value_; }
  [[nodiscard]] vdl::uint64_t GetCompletedValue()const { return pFence_->GetCompletedValue(); }
public:
  Fence() = default;

  void Initialize(ID3D12Device* _pDevice);

  //  現在の値を+1した値をGPUで更新するように設定
  void Signal(ID3D12CommandQueue* _pCommandQueue);

  //  現在の値になるまでCPUで待機
  void Wait(const HANDLE& _Event);

  //  現在の値になるまでGPUで待機
  void Wait(ID3D12CommandQueue* _pCommandQueue);
};
