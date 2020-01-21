#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Macro.hpp>

#include <vdl/pch/DirectX12/pch.hpp>

#include <assert.h>
#include <array>

class CommandQueueManager
{
  static constexpr D3D12_COMMAND_LIST_TYPE kCommandQueueTypes[] = {
    D3D12_COMMAND_LIST_TYPE_DIRECT,
    D3D12_COMMAND_LIST_TYPE_COMPUTE,
    D3D12_COMMAND_LIST_TYPE_COPY
  };
  static constexpr vdl::uint kCommandQueueNum = static_cast<vdl::uint>(vdl::Macro::ArraySize(kCommandQueueTypes));
private:
  std::array<Microsoft::WRL::ComPtr<ID3D12CommandQueue>, kCommandQueueNum> pCommandQueues_;
public:
  ID3D12CommandQueue* GetGraphicsQueue() { return pCommandQueues_[0].Get(); }
  ID3D12CommandQueue* GetComputeQueue() { return pCommandQueues_[1].Get(); }
  ID3D12CommandQueue* GetCopyQueue() { return pCommandQueues_[2].Get(); }
  ID3D12CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE _Type)
  {
    switch (_Type)
    {
    case D3D12_COMMAND_LIST_TYPE_DIRECT:
      return pCommandQueues_[0].Get();
    case D3D12_COMMAND_LIST_TYPE_COMPUTE:
      return pCommandQueues_[1].Get();
    case D3D12_COMMAND_LIST_TYPE_COPY:
      return pCommandQueues_[2].Get();
    default: assert(false);
    }

    return nullptr;
  }
public:
  CommandQueueManager() = default;

  ~CommandQueueManager() = default;

  void Initialize(ID3D12Device* _pDevice);
};
