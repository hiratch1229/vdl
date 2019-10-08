#pragma once
#include "../ITexture.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

struct CTexture : public ITexture
{
  vk::UniqueImage Image;
  vk::UniqueImageView View;
  vk::UniqueDeviceMemory Memory;
  vk::ImageLayout CurrentLayout = vk::ImageLayout::eUndefined;
  vk::Format Format;
  vdl::uint2 TextureSize;
public:
  CTexture() = default;

  TextureType GetType()const override { return TextureType::eTexture; }

  vdl::uint2 GetSize()const final { return TextureSize; }
public:
  void SetImageLayout(const vk::CommandBuffer& _CommandBuffer, vk::ImageLayout _NewImageLayout, const vk::ImageSubresourceRange& _SubresourceRange);
public:
  static void SetImageLayout(const vk::CommandBuffer& _CommandBuffer, const vk::Image& _Image,
    vk::ImageLayout _OldImageLayout, vk::ImageLayout _NewImageLayout, const vk::ImageSubresourceRange& _SubresourceRange);
};

struct CRenderTexture : public CTexture
{
public:
  CRenderTexture() = default;

  TextureType GetType()const override { return TextureType::eRenderTexture; }
};

struct CDepthStencilTexture : public CTexture
{
public:
  CDepthStencilTexture() = default;

  TextureType GetType()const override { return TextureType::eDepthStencilTexture; }
};

struct CUnorderedAccessTexture : public CTexture
{
public:
  CUnorderedAccessTexture() = default;

  TextureType GetType()const override { return TextureType::eUnorderedAccessTexture; }
};
