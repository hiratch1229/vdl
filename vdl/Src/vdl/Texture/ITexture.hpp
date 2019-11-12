#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Type2.hpp>
#include <vdl/Texture.hpp>

enum class TextureType
{
  eTexture,
  eCubeTexture,
  eRenderTexture,
  eSwapChainRenderTexture,
  eDepthStencilTexture,
  eDepthTexture,
  eStencilTexture,
  eUnorderedAccessTexture,
};

struct ITexture
{
  ITexture() = default;

  virtual ~ITexture() = default;

  [[nodiscard]] virtual TextureType GetType()const = 0;

  [[nodiscard]] virtual vdl::uint2 GetSize()const = 0;

  [[nodiscard]] virtual vdl::FormatType GetFormat()const = 0;
};

struct IDepthStencilTexture : public ITexture
{
public:
  IDepthStencilTexture() = default;

  [[nodiscard]] virtual const vdl::Texture& GetDepthTexture() = 0;

  [[nodiscard]] virtual const vdl::Texture& GetStencilTexture() = 0;
};
