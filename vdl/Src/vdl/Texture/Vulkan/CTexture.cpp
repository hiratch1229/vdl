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
    SrcStageMask = vk::PipelineStageFlagBits::eTransfer;
    break;
  case vk::ImageLayout::eColorAttachmentOptimal:
    break;
  case vk::ImageLayout::eDepthStencilAttachmentOptimal:
    break;
  case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
    break;
  case vk::ImageLayout::eShaderReadOnlyOptimal:
    break;
  case vk::ImageLayout::eTransferSrcOptimal:
    break;
  case vk::ImageLayout::eTransferDstOptimal:
    break;
  case vk::ImageLayout::ePresentSrcKHR:
    break;
  default: assert(false);
  }

  switch (_NewImageLayout)
  {
  case vk::ImageLayout::eColorAttachmentOptimal:
    DstStageMask = vk::PipelineStageFlagBits::eLateFragmentTests;
    break;
  case vk::ImageLayout::eDepthStencilAttachmentOptimal:
    break;
  case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
    break;
  case vk::ImageLayout::eShaderReadOnlyOptimal:
    DstStageMask = vk::PipelineStageFlagBits::eAllGraphics;
    ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
    break;
  case vk::ImageLayout::eTransferSrcOptimal:
    break;
  case vk::ImageLayout::eTransferDstOptimal:
    break;
  case vk::ImageLayout::ePresentSrcKHR:
    break;
  default: assert(false);
  }

  ////  ���݂̃��C�A�E�g
  //switch (_OldImageLayout)
  //{
  //  //  �J���[
  //case vk::ImageLayout::eColorAttachmentOptimal:
  //  SrcStageMask = vk::PipelineStageFlagBits::eAllGraphics;//TODO
  //  ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
  //  break;
  //  //  �[�x�E�X�e���V��
  //case vk::ImageLayout::eDepthStencilAttachmentOptimal:
  //  SrcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
  //  ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
  //  break;
  //  //  �V�F�[�_���\�[�X
  //case vk::ImageLayout::eShaderReadOnlyOptimal:
  //  SrcStageMask = vk::PipelineStageFlagBits::eFragmentShader;
  //  ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
  //  break;
  //  //  �]����
  //case vk::ImageLayout::eTransferSrcOptimal:
  //  ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
  //  break;
  //  //  �]����
  //case vk::ImageLayout::eTransferDstOptimal:
  //  ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
  //  break;
  //  //  �������
  //case vk::ImageLayout::ePreinitialized:
  //  ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
  //  break;
  //case vk::ImageLayout::ePresentSrcKHR:
  //  ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
  //  break;
  //default:
  //  break;
  //}
  //
  ////  ���̃��C�A�E�g
  //switch (_NewImageLayout)
  //{
  //  //  �J���[
  //case vk::ImageLayout::eColorAttachmentOptimal:
  //  ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
  //  DstStageMask = vk::PipelineStageFlagBits::eAllGraphics;//TODO
  //  break;
  //  //  �[�x�E�X�e���V��
  //case vk::ImageLayout::eDepthStencilAttachmentOptimal:
  //  ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
  //  DstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
  //
  //  if (_OldImageLayout == vk::ImageLayout::eUndefined)
  //  {
  //    SrcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
  //  }
  //  break;
  //  //  �V�F�[�_���\�[�X
  //case vk::ImageLayout::eShaderReadOnlyOptimal:
  //  //if (ImageMemoryBarrier.srcAccessMask == vk::AccessFlags())
  //  //{
  //  //  ImageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite;
  //  //}
  //
  //  ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
  //  DstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
  //  break;
  //  //  �]����
  //case vk::ImageLayout::eTransferSrcOptimal:
  //  ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
  //  break;
  //  //  �]����
  //case vk::ImageLayout::eTransferDstOptimal:
  //  ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
  //  break;
  //case vk::ImageLayout::ePresentSrcKHR:
  //  ImageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
  //  break;
  //default:
  //  break;
  //}

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
