#include "CTexture.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>

#include <vdl/Device/DirectX12/CommandList/CommandList.hpp>

#include <vdl/Format/Format.hpp>

void CTexture::TransitionResourceBarrier(CommandList* _pCommandList, ID3D12Resource* _pResource,
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

  (*_pCommandList)->ResourceBarrier(1, &ResourceBarrier);
}

//--------------------------------------------------

void TextureData::TransitionResourceBarrier(CommandList* _pCommandList, D3D12_RESOURCE_STATES _AfterState)
{
  CTexture::TransitionResourceBarrier(_pCommandList, pResource.Get(), ResourceState, _AfterState);
  ResourceState = _AfterState;
}

//--------------------------------------------------

const vdl::Texture& CDepthStencilTexture::GetDepthTexture()
{
  if (DepthTexture.isEmpty())
  {
    DepthTexture = vdl::Palette::White;

    ITexture* pDepthTexture;
    Engine::Get<IDevice>()->CreateDepthTexture(&pDepthTexture, this);

    ITextureManager* pTextureManager = Engine::Get<ITextureManager>();
    pTextureManager->SetTexture(DepthTexture.GetID(), pDepthTexture);

    //  TODO:�e�̃I�u�W�F�N�g��n���ĎQ�ƃJ�E���g�𑝂₷
    for (vdl::uint i = 0;; ++i)
    {
      if (this == pTextureManager->GetTexture(i))
      {
        pTextureManager->AddRef(i);
        break;
      }
    }
  }

  return DepthTexture;
}

const vdl::Texture& CDepthStencilTexture::GetStencilTexture()
{
  if (hasStencil(Format) && StencilTexture.isEmpty())
  {
    StencilTexture = vdl::Palette::White;

    ITexture* pStencilTexture;
    Engine::Get<IDevice>()->CreateStencilTexture(&pStencilTexture, this);

    ITextureManager* pTextureManager = Engine::Get<ITextureManager>();
    pTextureManager->SetTexture(DepthTexture.GetID(), pStencilTexture);

    //  TODO:�e�̃I�u�W�F�N�g��n���ĎQ�ƃJ�E���g�𑝂₷
    for (vdl::uint i = 0;; ++i)
    {
      if (this == pTextureManager->GetTexture(i))
      {
        pTextureManager->AddRef(i);
        break;
      }
    }
  }

  return StencilTexture;
}

//--------------------------------------------------

CDepthTexture::~CDepthTexture()
{
  assert(pParent);

  ITextureManager* pTextureManager = Engine::Get<ITextureManager>();
  for (vdl::uint i = 0;; ++i)
  {
    if (pParent == pTextureManager->GetTexture(i))
    {
      pTextureManager->Release(i);
      break;
    }
  }
}

//--------------------------------------------------

CStencilTexture::~CStencilTexture()
{
  assert(pParent);

  ITextureManager* pTextureManager = Engine::Get<ITextureManager>();
  for (vdl::uint i = 0;; ++i)
  {
    if (pParent == pTextureManager->GetTexture(i))
    {
      pTextureManager->Release(i);
      break;
    }
  }
}
