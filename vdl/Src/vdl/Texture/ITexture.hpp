#pragma once
#include <vdl/Type2.hpp>

enum class TextureType
{
  eTexture,
  eRenderTexture,
  eDepthStencilTexture,
  eUnorderedAccessTexture,
};

struct ITexture
{
  ITexture() = default;

  virtual ~ITexture() = default;

  [[nodiscard]] virtual TextureType GetType()const = 0;

  [[nodiscard]] virtual vdl::uint2 GetSize()const = 0;
};
