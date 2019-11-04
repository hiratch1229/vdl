#pragma once
#include "../ITexture.hpp"

#include <vdl/pch/Vulkan/pch.hpp>

#include <vdl/Constants/Constants.hpp>

struct CTexture : public ITexture
{
  vk::UniqueImage Image;
  vk::UniqueImageView View;
  vk::UniqueDeviceMemory Memory;
  vk::ImageLayout CurrentLayout = vk::ImageLayout::eUndefined;
  vdl::uint2 TextureSize;
public:
  CTexture() = default;

  TextureType GetType()const override { return TextureType::eTexture; }

  vdl::uint2 GetSize()const final { return TextureSize; }

  vdl::FormatType GetFormat()const override { return Constants::kTextureFormat; }
public:
  void SetImageLayout(const vk::CommandBuffer& _CommandBuffer, vk::ImageLayout _NewImageLayout, const vk::ImageSubresourceRange& _SubresourceRange);
public:
  static void SetImageLayout(const vk::CommandBuffer& _CommandBuffer, const vk::Image& _Image,
    vk::ImageLayout _OldImageLayout, vk::ImageLayout _NewImageLayout, const vk::ImageSubresourceRange& _SubresourceRange);
};

struct CRenderTexture : public CTexture
{
  vk::Format VkFormat;
  vdl::FormatType Format;
public:
  CRenderTexture() = default;

  TextureType GetType()const override { return TextureType::eRenderTexture; }

  vdl::FormatType GetFormat()const final { return Format; }
};

struct CDepthStencilTexture : public IDepthStencilTexture
{
  vk::UniqueImage Image;
  vk::UniqueImageView View;
  vk::UniqueDeviceMemory Memory;
  vk::ImageLayout CurrentLayout = vk::ImageLayout::eUndefined;
  vdl::uint2 TextureSize;
  vdl::Texture DepthTexture;
  vdl::Texture StencilTexture;
  vk::Format VkFormat;
  vdl::FormatType Format;
  vk::ImageAspectFlags ImageAspectFlag;
public:
  CDepthStencilTexture() = default;

  TextureType GetType()const override { return TextureType::eDepthStencilTexture; }

  vdl::uint2 GetSize()const final { return TextureSize; }

  vdl::FormatType GetFormat()const final { return Format; }

  vdl::Texture GetDepthTexture()final;

  vdl::Texture GetStencilTexture()final;
public:
  void SetImageLayout(const vk::CommandBuffer& _CommandBuffer, vk::ImageLayout _NewImageLayout, const vk::ImageSubresourceRange& _SubresourceRange);
};

struct CDepthTexture : public ITexture
{
  CDepthStencilTexture* pParent;
  vk::UniqueImageView View;
public:
  CDepthTexture() = default;

  TextureType GetType()const final { return TextureType::eDepthTexture; }

  vdl::uint2 GetSize()const final { return pParent->TextureSize; }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eUnknown; }
};

struct CStencilTexture : public ITexture
{
  CDepthStencilTexture* pParent;
  vk::UniqueImageView View;
public:
  CStencilTexture() = default;

  TextureType GetType()const final { return TextureType::eStencilTexture; }

  vdl::uint2 GetSize()const final { return pParent->TextureSize; }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eUnknown; }
};

struct CUnorderedAccessTexture : public CTexture
{
  vdl::FormatType Format;
public:
  CUnorderedAccessTexture() = default;

  TextureType GetType()const override { return TextureType::eUnorderedAccessTexture; }

  vdl::FormatType GetFormat()const final { return Format; }
};
