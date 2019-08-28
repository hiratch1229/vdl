#pragma once
#include "ID.hpp"
#include "Color.hpp"
#include "Image.hpp"
#include "Format.hpp"

namespace vdl
{
  class Texture
  {
  protected:
    ID ID_;
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }
    [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }
    [[nodiscard]] uint2 GetSize()const;
  public:
    [[nodiscard]] constexpr bool operator==(const Texture& _Texture)const noexcept { return ID_ == _Texture.ID_; }
    [[nodiscard]] constexpr bool operator!=(const Texture& _Texture)const noexcept { return ID_ != _Texture.ID_; }
  public:
    Texture() = default;

    virtual ~Texture();

    //  ファイルから作成
    Texture(const char* _FilePath);

    //  色情報から作成
    Texture(const ColorF& _Color);

    Texture(const Image& _Image);

    Texture(const Texture& _Texture);

    Texture& operator=(const Texture& _Texture);
  };

  class RenderTexture : public Texture
  {
  public:
    RenderTexture() = default;

    RenderTexture(const uint2& _TextureSize, Format _Format);

    void Clear(const ColorF& _Color = Palette::Black);
  };

  class DepthStencilTexture : public Texture
  {
  public:
    DepthStencilTexture() = default;

    DepthStencilTexture(const uint2& _TextureSize, Format _Format);

    void Clear(float _Depth = 1.0f, uint _Stencil = 0);
  };
}
