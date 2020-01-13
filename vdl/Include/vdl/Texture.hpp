#pragma once
#include "Fwd.hpp"
#include "Color.hpp"
#include "Constants.hpp"
#include "Resource.hpp"

namespace vdl
{
  class Texture : public Resource
  {
  public:
    Texture() = default;

    //  �t�@�C������쐬
    Texture(const char* _FilePath);

    //  �f�[�^����쐬
    Texture(const Image& _Image);

    //  �F��񂩂�쐬
    Texture(const Color3& _Color, uint8_t _Alpha = 255);

    //  �F��񂩂�쐬
    Texture(const Color4& _Color);

    Texture(const Texture& _Texture);

    Texture(Texture&& _Texture)noexcept;

    virtual ~Texture();
  public:
    Texture& operator=(const Texture& _Texture);

    Texture& operator=(Texture&& _Texture)noexcept;

    [[nodiscard]] bool operator==(const Texture& _Texture)const noexcept { return ID_ == _Texture.ID_; }

    [[nodiscard]] bool operator!=(const Texture& _Texture)const noexcept { return ID_ != _Texture.ID_; }
  public:
    [[nodiscard]] uint2 GetSize()const;
  };

  class RenderTexture : public Texture
  {
  public:
    RenderTexture() = default;

    RenderTexture(const uint2& _TextureSize, FormatType _Format);
  public:
    [[nodiscard]] FormatType GetFormat()const;
  };

  class UnorderedAccessTexture : public Texture
  {
  public:
    UnorderedAccessTexture() = default;

    UnorderedAccessTexture(const uint2& _TextureSize, FormatType _Format);
  public:
    [[nodiscard]] FormatType GetFormat()const;
  };

  class DepthStencilTexture : public Resource
  {
  public:
    DepthStencilTexture() = default;

    DepthStencilTexture(const uint2& _TextureSize, FormatType _Format);

    DepthStencilTexture(const DepthStencilTexture& _DepthStencilBuffer);

    DepthStencilTexture(DepthStencilTexture&& _DepthStencilBuffer)noexcept;

    ~DepthStencilTexture();
  public:
    DepthStencilTexture& operator=(const DepthStencilTexture& _DepthStencilTexture);

    DepthStencilTexture& operator=(DepthStencilTexture&& _DepthStencilTexture)noexcept;

    [[nodiscard]] bool operator==(const DepthStencilTexture& _DepthStencilTexture)const noexcept { return ID_ == _DepthStencilTexture.ID_; }

    [[nodiscard]] bool operator!=(const DepthStencilTexture& _DepthStencilTexture)const noexcept { return ID_ != _DepthStencilTexture.ID_; }
  public:
    [[nodiscard]] FormatType GetFormat()const;

    [[nodiscard]] vdl::Texture GetDepthTexture()const;

    [[nodiscard]] vdl::Texture GetStencilTexture()const;
  };

  class CubeTexture : public Resource
  {
  public:
    CubeTexture() = default;

    //  �t�@�C������쐬
    CubeTexture(const char* _FilePath);

    //  �f�[�^����쐬
    CubeTexture(const Image& _Image);

    CubeTexture(const CubeTexture& _CubeTexture);

    CubeTexture(CubeTexture&& _CubeTexture)noexcept;

    virtual ~CubeTexture();
  public:
    CubeTexture& operator=(const CubeTexture& _CubeTexture);

    CubeTexture& operator=(CubeTexture&& _CubeTexture)noexcept;

    [[nodiscard]] bool operator==(const CubeTexture& _CubeTexture)const noexcept { return ID_ == _CubeTexture.ID_; }

    [[nodiscard]] bool operator!=(const CubeTexture& _CubeTexture)const noexcept { return ID_ != _CubeTexture.ID_; }
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
