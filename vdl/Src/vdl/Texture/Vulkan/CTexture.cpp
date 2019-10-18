#include "CTexture.hpp"

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
  case vk::ImageLayout::eColorAttachmentOptimal:
    SrcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    break;
  case vk::ImageLayout::eDepthStencilAttachmentOptimal:
    SrcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    break;
  case vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal:
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
  case vk::ImageLayout::eDepthStencilAttachmentOptimal:
    DstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
    ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    break;
  case vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal:
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
  if (_NewImageLayout == CurrentLayout)
  {
    return;
  }

  CTexture::SetImageLayout(_CommandBuffer, Image.get(), CurrentLayout, _NewImageLayout, _SubresourceRange);
  CurrentLayout = _NewImageLayout;
}
