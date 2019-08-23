#pragma once

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

  virtual TextureType GetType()const = 0;
};
