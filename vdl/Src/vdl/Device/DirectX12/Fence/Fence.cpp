#include "Fence.hpp"

#include <vdl/Misc/Windows/Misc.hpp>

void Fence::Initialize(ID3D12Device* _pDevice)
{
  HRESULT hr = S_OK;

  hr = _pDevice->CreateFence(Value_ = 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(pFence_.GetAddressOf()));
  ERROR_CHECK(hr);
}

void Fence::Signal(ID3D12CommandQueue* _pCommandQueue)
{
  HRESULT hr = S_OK;

  hr = _pCommandQueue->Signal(pFence_.Get(), ++Value_);
  ERROR_CHECK(hr);
}

void Fence::Wait(const HANDLE& _Event)
{
  HRESULT hr = S_OK;

  if (pFence_->GetCompletedValue() < Value_)
  {
    hr = pFence_->SetEventOnCompletion(Value_, _Event);
    ERROR_CHECK(hr);
    ::WaitForSingleObjectEx(_Event, INFINITE, false);
  }
}

void Fence::Wait(ID3D12CommandQueue* _pCommandQueue)
{
  HRESULT hr = S_OK;

  hr = _pCommandQueue->Wait(pFence_.Get(), Value_);
  ERROR_CHECK(hr);
}
