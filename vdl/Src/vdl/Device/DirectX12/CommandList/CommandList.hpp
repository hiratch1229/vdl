#pragma once
#include <vdl/pch/DirectX12/pch.hpp>

class CommandList
{
  enum class State
  {
    eClose,
    eBegin,
  };
private:
  State State_;
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pCommandAllocator_;
  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> pCommandList_;
public:
  [[nodiscard]] bool isClose()const { return State_ == State::eClose; }

  [[nodiscard]] ID3D12GraphicsCommandList4* Get()const { return pCommandList_.Get(); }

  ID3D12GraphicsCommandList4* operator->()const { return pCommandList_.Get(); }
public:
  CommandList() = default;

  void Initialize(ID3D12Device4* _pDevice, D3D12_COMMAND_LIST_TYPE _Type);

  void Reset(ID3D12PipelineState* _pPipelineState = nullptr);

  void Close();

  void Execute(ID3D12CommandQueue* _pCommandQueue);
};
