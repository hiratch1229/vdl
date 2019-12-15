#include "CTexture.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>

#include <vdl/Format/Format.hpp>

const vdl::Texture& CDepthStencilTexture::GetDepthTexture()
{
  if (DepthTexture.isEmpty())
  {
    DepthTexture = vdl::Palette::White;

    ITexture* pDepthTexture;
    Engine::Get<IDevice>()->CreateDepthTexture(&pDepthTexture, this);

    ITextureManager* pTextureManager = Engine::Get<ITextureManager>();
    pTextureManager->SetTexture(DepthTexture.GetID(), pDepthTexture);

    //  TODO:親のオブジェクトを渡して参照カウントを増やす
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

    //  TODO:親のオブジェクトを渡して参照カウントを増やす
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
