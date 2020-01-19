#include "CommandQueueManager.hpp"

#include <vdl/Misc/Windows/Misc.hpp>

void CommandQueueManager::Initialize(ID3D12Device* _pDevice)
{
  //  エラーチェック用
  HRESULT hr = S_OK;

  D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
  {
    CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
    CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    CommandQueueDesc.NodeMask = 0;
  }

  for (vdl::uint i = 0; i < kCommandQueueNum; ++i)
  {
    CommandQueueDesc.Type = kCommandQueueTypes[i];
    hr = _pDevice->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(pCommandQueues_[i].GetAddressOf()));
    ERROR_CHECK(hr);
  }
}
