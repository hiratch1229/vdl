#include "CommandList.hpp"

#include <vdl/Misc/Windows/Misc.hpp>

#include <assert.h>

CommandList::CommandList(ID3D12Device4* _pDevice, D3D12_COMMAND_LIST_TYPE _Type)
{
  HRESULT hr = S_OK;

  //  コマンドアロケータの作成
  {
    hr = _pDevice->CreateCommandAllocator(_Type, IID_PPV_ARGS(pCommandAllocator_.GetAddressOf()));
    ERROR_CHECK(hr);
  }

  //  コマンドリストの作成
  {
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList1> pCommandList;
    hr = _pDevice->CreateCommandList1(0, _Type, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(pCommandList.GetAddressOf()));
    ERROR_CHECK(hr);

    hr = pCommandList.As(&pCommandList_);
    ERROR_CHECK(hr);

    State_ = State::eClose;
  }
}

void CommandList::Reset(ID3D12PipelineState* _pPipelineState)
{
  assert(State_ == State::eClose);

  HRESULT hr = S_OK;

  hr = pCommandAllocator_->Reset();
  ERROR_CHECK(hr);

  hr = pCommandList_->Reset(pCommandAllocator_.Get(), _pPipelineState);
  ERROR_CHECK(hr);

  State_ = State::eBegin;
}

void CommandList::Close()
{
  assert(State_ == State::eBegin);

  HRESULT hr = S_OK;

  hr = pCommandList_->Close();
  ERROR_CHECK(hr);

  State_ = State::eClose;
}
