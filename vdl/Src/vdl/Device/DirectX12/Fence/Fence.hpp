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

  //  ���݂̒l��+1�����l��GPU�ōX�V����悤�ɐݒ�
  void Signal(ID3D12CommandQueue* _pCommandQueue);

  //  ���݂̒l�ɂȂ�܂�CPU�őҋ@
  void Wait(const HANDLE& _Event);

  //  ���݂̒l�ɂȂ�܂�GPU�őҋ@
  void Wait(ID3D12CommandQueue* _pCommandQueue);
};
