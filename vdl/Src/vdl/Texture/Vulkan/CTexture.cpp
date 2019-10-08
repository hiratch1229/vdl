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

  vk::PipelineStageFlags SrcStageMask = vk::PipelineStageFlagBits::eTransfer;
  vk::PipelineStageFlags DstStageMask = vk::PipelineStageFlagBits::eTransfer;

  //  現在のレイアウト
  switch (_OldImageLayout)
  {
    //  カラー
  case vk::ImageLayout::eColorAttachmentOptimal:
    SrcStageMask = vk::PipelineStageFlagBits::eAllGraphics;//TODO
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    break;
    //  深度・ステンシル
  case vk::ImageLayout::eDepthStencilAttachmentOptimal:
    SrcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    break;
    //  シェーダリソース
  case vk::ImageLayout::eShaderReadOnlyOptimal:
    SrcStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
    break;
    //  転送元
  case vk::ImageLayout::eTransferSrcOptimal:
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
    break;
    //  転送先
  case vk::ImageLayout::eTransferDstOptimal:
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    break;
    //  初期状態
  case vk::ImageLayout::ePreinitialized:
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
    break;
  case vk::ImageLayout::ePresentSrcKHR:
    ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
    break;
  default:
    break;
  }

  //  次のレイアウト
  switch (_NewImageLayout)
  {
    //  カラー
  case vk::ImageLayout::eColorAttachmentOptimal:
    ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    DstStageMask = vk::PipelineStageFlagBits::eAllGraphics;//TODO
    break;
    //  深度・ステンシル
  case vk::ImageLayout::eDepthStencilAttachmentOptimal:
    ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    DstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;

    if (_OldImageLayout == vk::ImageLayout::eUndefined)
    {
      SrcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
    }
    break;
    //  シェーダリソース
  case vk::ImageLayout::eShaderReadOnlyOptimal:
    //if (ImageMemoryBarrier.srcAccessMask == vk::AccessFlags())
    //{
    //  ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite;
    //}

    ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
    DstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    break;
    //  転送元
  case vk::ImageLayout::eTransferSrcOptimal:
    ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
    break;
    //  転送先
  case vk::ImageLayout::eTransferDstOptimal:
    ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
    break;
  case vk::ImageLayout::ePresentSrcKHR:
    ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    break;
  default:
    break;
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
