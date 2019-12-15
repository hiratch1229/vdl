#include "CTexture.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>

#include <vdl/Format/Format.hpp>

void CTexture::SetImageLayout(const vk::CommandBuffer& _CommandBuffer, const vk::Image& _Image,
  vk::ImageLayout _OldImageLayout, vk::ImageLayout _NewImageLayout, const vk::ImageSubresourceRange& _SubresourceRange)
{
  vk::ImageMemoryBarrier ImageMemoryBarrier;
  {
    ImageMemoryBarrier.oldLayout = _OldImageLayout;
    ImageMemoryBarrier.newLayout = _NewImageLayout;
    ImageMemoryBarrier.image = _Image;
    ImageMemoryBarrier.subresourceRange = _SubresourceRange;
  }

  vk::PipelineStageFlags SrcStageMask;
  vk::PipelineStageFlags DstStageMask;

  switch (_OldImageLayout)
  {
  case vk::ImageLayout::eUndefined:
    SrcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
    break;
  case vk::ImageLayout::eGeneral:
    SrcStageMask = vk::PipelineStageFlagBits::eComputeShader;
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;
    break;
  case vk::ImageLayout::eColorAttachmentOptimal:
    SrcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    break;
  case vk::ImageLayout::eDepthStencilAttachmentOptimal:
    SrcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    break;
  case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
    SrcStageMask = vk::PipelineStageFlagBits::eAllGraphics;
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead;
    break;
  case vk::ImageLayout::eShaderReadOnlyOptimal:
    SrcStageMask = vk::PipelineStageFlagBits::eAllGraphics;
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
    break;
  case vk::ImageLayout::eTransferDstOptimal:
    SrcStageMask = vk::PipelineStageFlagBits::eTransfer;
    break;
  case vk::ImageLayout::ePresentSrcKHR:
    SrcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    break;
  default: assert(false);
  }

  switch (_NewImageLayout)
  {
  case vk::ImageLayout::eColorAttachmentOptimal:
    DstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    break;
  case vk::ImageLayout::eGeneral:
    DstStageMask = vk::PipelineStageFlagBits::eComputeShader;
    ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;
    break;
  case vk::ImageLayout::eDepthStencilAttachmentOptimal:
    DstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
    ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    break;
  case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
    DstStageMask = vk::PipelineStageFlagBits::eAllGraphics;
    ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead;
    break;
  case vk::ImageLayout::eShaderReadOnlyOptimal:
    DstStageMask = vk::PipelineStageFlagBits::eAllGraphics;
    ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
    break;
  case vk::ImageLayout::eTransferDstOptimal:
    DstStageMask = vk::PipelineStageFlagBits::eTransfer;
    break;
  case vk::ImageLayout::ePresentSrcKHR:
    DstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    break;
  default: assert(false);
  }

  _CommandBuffer.pipelineBarrier(SrcStageMask, DstStageMask,
    vk::DependencyFlags(), nullptr, nullptr, ImageMemoryBarrier);
}

//--------------------------------------------------

void TextureData::SetImageLayout(const vk::CommandBuffer& _CommandBuffer, vk::ImageLayout _NewImageLayout, const vk::ImageSubresourceRange& _SubresourceRange)
{
  assert(_NewImageLayout != CurrentLayout);

  CTexture::SetImageLayout(_CommandBuffer, Image.get(), CurrentLayout, _NewImageLayout, _SubresourceRange);
  CurrentLayout = _NewImageLayout;
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
