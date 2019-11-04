#include "CTexture.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/Vulkan/CDevice.hpp>
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

void CTexture::SetImageLayout(const vk::CommandBuffer& _CommandBuffer, vk::ImageLayout _NewImageLayout, const vk::ImageSubresourceRange& _SubresourceRange)
{
  assert(_NewImageLayout != CurrentLayout);

  CTexture::SetImageLayout(_CommandBuffer, Image.get(), CurrentLayout, _NewImageLayout, _SubresourceRange);
  CurrentLayout = _NewImageLayout;
}

//--------------------------------------------------

vdl::Texture CDepthStencilTexture::GetDepthTexture()
{
  if (DepthTexture.isEmpty())
  {
    DepthTexture = vdl::Palette::White;

    CDepthTexture* pDepthTexture = new CDepthTexture;
    pDepthTexture->pParent = this;

    vk::ImageViewCreateInfo ImageViewInfo;
    {
      ImageViewInfo.image = Image.get();
      ImageViewInfo.viewType = vk::ImageViewType::e2D;
      ImageViewInfo.format = VkFormat;
      ImageViewInfo.components.r = vk::ComponentSwizzle::eR;
      ImageViewInfo.components.g = vk::ComponentSwizzle::eG;
      ImageViewInfo.components.b = vk::ComponentSwizzle::eB;
      ImageViewInfo.components.a = vk::ComponentSwizzle::eA;
      ImageViewInfo.subresourceRange = { vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1 };
    }

    pDepthTexture->View = static_cast<CDevice*>(Engine::Get<IDevice>())->GetDevice().createImageViewUnique(ImageViewInfo);
    assert(pDepthTexture->View);

    Engine::Get<ITextureManager>()->SetTexture(DepthTexture.GetID(), pDepthTexture);
  }

  return DepthTexture;
}

vdl::Texture CDepthStencilTexture::GetStencilTexture()
{
  if (hasStencil(Format) && StencilTexture.isEmpty())
  {
    StencilTexture = vdl::Palette::White;

    CStencilTexture* pStencilTexture = new CStencilTexture;
    pStencilTexture->pParent = this;

    vk::ImageViewCreateInfo ImageViewInfo;
    {
      ImageViewInfo.image = Image.get();
      ImageViewInfo.viewType = vk::ImageViewType::e2D;
      ImageViewInfo.format = VkFormat;
      ImageViewInfo.components.r = vk::ComponentSwizzle::eR;
      ImageViewInfo.components.g = vk::ComponentSwizzle::eG;
      ImageViewInfo.components.b = vk::ComponentSwizzle::eB;
      ImageViewInfo.components.a = vk::ComponentSwizzle::eA;
      ImageViewInfo.subresourceRange = { vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1 };
    }

    pStencilTexture->View = static_cast<CDevice*>(Engine::Get<IDevice>())->GetDevice().createImageViewUnique(ImageViewInfo);
    assert(pStencilTexture->View);

    Engine::Get<ITextureManager>()->SetTexture(StencilTexture.GetID(), pStencilTexture);
  }

  return StencilTexture;
}

void CDepthStencilTexture::SetImageLayout(const vk::CommandBuffer & _CommandBuffer, vk::ImageLayout _NewImageLayout, const vk::ImageSubresourceRange & _SubresourceRange)
{
  assert(_NewImageLayout != CurrentLayout);

  CTexture::SetImageLayout(_CommandBuffer, Image.get(), CurrentLayout, _NewImageLayout, _SubresourceRange);
  CurrentLayout = _NewImageLayout;
}
