#pragma once
#include "../ITexture.hpp"

#include <vdl/pch/Vulkan/pch.hpp>

#include <vdl/Window.hpp>

#include <vdl/Constants/Constants.hpp>

struct TextureData
{
  vk::UniqueImage Image;
  vk::UniqueImageView View;
  vk::UniqueDeviceMemory Memory;
  vk::ImageLayout CurrentLayout = vk::ImageLayout::eUndefined;
public:
  TextureData() = default;

  TextureData(TextureData&& _Data)noexcept
    : Image(std::move(_Data.Image)), View(std::move(_Data.View)), Memory(std::move(_Data.Memory))
    , CurrentLayout(std::move(_Data.CurrentLayout)) {}

  TextureData& operator=(TextureData&& _Data)noexcept
  {
    Image = std::move(_Data.Image);
    View = std::move(_Data.View);
    Memory = std::move(_Data.Memory);
    CurrentLayout = std::move(_Data.CurrentLayout);

    return *this;
  }
public:
  void SetImageLayout(const vk::CommandBuffer& _CommandBuffer, vk::ImageLayout _NewImageLayout, const vk::ImageSubresourceRange& _SubresourceRange);
};

struct CTexture : public ITexture
{
  TextureData TextureData;
  vdl::uint2 TextureSize;
public:
  CTexture() = default;

  TextureType GetType()const override { return TextureType::eTexture; }

  vdl::uint2 GetSize()const final { return TextureSize; }

  vdl::FormatType GetFormat()const override { return Constants::kTextureFormat; }
public:
  static void SetImageLayout(const vk::CommandBuffer& _CommandBuffer, const vk::Image& _Image,
    vk::ImageLayout _OldImageLayout, vk::ImageLayout _NewImageLayout, const vk::ImageSubresourceRange& _SubresourceRange);
};

struct CCubeTexture : public CTexture
{
public:
  CCubeTexture() = default;

  TextureType GetType()const final { return TextureType::eCubeTexture; }
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

struct CSwapChainRenderTexture : public ITexture
{
public:
  CSwapChainRenderTexture() = default;

  TextureType GetType()const final { return TextureType::eSwapChainRenderTexture; }

  vdl::uint2 GetSize()const final { return vdl::Window::GetWindowSize(); }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eSwapChain; }
};

struct CDepthStencilTexture : public IDepthStencilTexture
{
  TextureData TextureData;
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

  const vdl::Texture& GetDepthTexture()final;

  const vdl::Texture& GetStencilTexture()final;
};

struct CDepthTexture : public ITexture
{
  CDepthStencilTexture* pParent;
  vk::UniqueImageView View;
public:
  CDepthTexture() = default;

  ~CDepthTexture();

  TextureType GetType()const final { return TextureType::eDepthTexture; }

  vdl::uint2 GetSize()const final { return pParent->GetSize(); }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eUnknown; }
};

struct CStencilTexture : public ITexture
{
  CDepthStencilTexture* pParent;
  vk::UniqueImageView View;
public:
  CStencilTexture() = default;

  ~CStencilTexture();

  TextureType GetType()const final { return TextureType::eStencilTexture; }

  vdl::uint2 GetSize()const final { return pParent->GetSize(); }

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

template<class Texture>
inline Texture* Cast(ITexture* _pTexture)
{
  static_assert(std::is_base_of<ITexture, Texture>::value);

  if constexpr (std::is_same<CTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eTexture || _pTexture->GetType() == TextureType::eCubeTexture || _pTexture->GetType() == TextureType::eRenderTexture || _pTexture->GetType() == TextureType::eDepthTexture
      || _pTexture->GetType() == TextureType::eStencilTexture || _pTexture->GetType() == TextureType::eUnorderedAccessTexture);
  }
  if constexpr (std::is_same<CCubeTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eCubeTexture);
  }
  if constexpr (std::is_same<CRenderTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eRenderTexture);
  }
  if constexpr (std::is_same<CSwapChainRenderTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eSwapChainRenderTexture);
  }
  if constexpr (std::is_same<CDepthStencilTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eDepthStencilTexture);
  }
  if constexpr (std::is_same<CDepthTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eDepthTexture);
  }
  if constexpr (std::is_same<CStencilTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eStencilTexture);
  }
  if constexpr (std::is_same<CUnorderedAccessTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eUnorderedAccessTexture);
  }

  return static_cast<Texture*>(_pTexture);
}
