#pragma once
#include "Fwd.hpp"
#include "Color.hpp"
#include "Constants.hpp"

namespace vdl
{
  class Texture
  {
  protected:
    ID ID_;
  public:
    Texture() = default;

    //  ファイルから作成
    Texture(const char* _FilePath, bool _isSerialize = true);

    //  データから作成
    Texture(const Image& _Image);

    //  色情報から作成
    Texture(const Color& _Color);

    Texture(const Texture& _Texture);

    Texture(Texture&& _Texture)noexcept;

    virtual ~Texture();
  public:
    Texture& operator=(const Texture& _Texture);

    Texture& operator=(Texture&& _Texture)noexcept;

    [[nodiscard]] constexpr bool operator==(const Texture& _Texture)const noexcept { return ID_ == _Texture.ID_; }

    [[nodiscard]] constexpr bool operator!=(const Texture& _Texture)const noexcept { return ID_ != _Texture.ID_; }
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }

    [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }

    [[nodiscard]] uint2 GetSize()const;
  };

  class RenderTexture : public Texture
  {
  public:
    RenderTexture() = default;

    RenderTexture(const uint2& _TextureSize, FormatType _Format);
  };

  class DepthStencilTexture : public Texture
  {
  public:
    DepthStencilTexture() = default;

    DepthStencilTexture(const uint2& _TextureSize, FormatType _Format);
  };

  class UnorderedAccessTexture : public Texture
  {
  public:
    UnorderedAccessTexture() = default;

    UnorderedAccessTexture(const uint2& _TextureSize, FormatType _Format);
  };

  struct OutputManager
  {
    vdl::RenderTextures RenderTextures;
    vdl::DepthStencilTexture DepthStencilTexture;
  public:
    [[nodiscard]] bool operator==(const OutputManager& _OutputManager)const { return RenderTextures == _OutputManager.RenderTextures && DepthStencilTexture == _OutputManager.DepthStencilTexture; }

    [[nodiscard]] bool operator!=(const OutputManager& _OutputManager)const { return RenderTextures != _OutputManager.RenderTextures || DepthStencilTexture != _OutputManager.DepthStencilTexture; }
  };
}
