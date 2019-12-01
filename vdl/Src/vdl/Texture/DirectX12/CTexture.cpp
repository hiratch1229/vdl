#include "CTexture.hpp"

void CTexture::TransitionResourceBarrier(ID3D12GraphicsCommandList* _pCommandList, ID3D12Resource* _pResource,
  D3D12_RESOURCE_STATES _BeforeState, D3D12_RESOURCE_STATES _AfterState)
{
  D3D12_RESOURCE_BARRIER ResourceBarrier;
  {
    ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    ResourceBarrier.Transition.pResource = _pResource;
    ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    ResourceBarrier.Transition.StateBefore = _BeforeState;
    ResourceBarrier.Transition.StateAfter = _AfterState;
  }

  _pCommandList->ResourceBarrier(1, &ResourceBarrier);
}

//--------------------------------------------------

void TextureData::TransitionResourceBarrier(ID3D12GraphicsCommandList* _pCommandList, D3D12_RESOURCE_STATES _AfterState)
{
  CTexture::TransitionResourceBarrier(_pCommandList, pResource.Get(), ResourceState, _AfterState);
  ResourceState = _AfterState;
}
