#pragma once
#include <vdl/Fwd.hpp>

#include <vdl/Texture/ITexture.hpp>

class ITextureManager
{
public:
  static ITextureManager* Create();

  ITextureManager() = default;

  virtual ~ITextureManager() = default;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual vdl::ID LoadTexture(const char* _FilePath, bool _isSerialize) = 0;

  [[nodiscard]] virtual vdl::ID LoadTexture(const vdl::Image& _Image) = 0;

  [[nodiscard]] virtual vdl::ID LoadCubeTexture(const char* _FilePath, bool _isSerialize) = 0;

  [[nodiscard]] virtual vdl::ID LoadCubeTexture(const vdl::Image& _Image) = 0;

  [[nodiscard]] virtual vdl::ID CreateRenderTexture(const vdl::uint2& _TextureSize, vdl::FormatType _Format) = 0;

  [[nodiscard]] virtual vdl::ID CreateDepthStencilTexture(const vdl::uint2& _TextureSize, vdl::FormatType _Format) = 0;

  [[nodiscard]] virtual vdl::ID CreateUnorderedAccessTexture(const vdl::uint2& _TextureSize, vdl::FormatType _Format) = 0;

  virtual void AddRef(const vdl::ID& _ID) = 0;

  virtual void Release(const vdl::ID& _ID) = 0;

  [[nodiscard]] virtual ITexture* GetTexture(const vdl::ID& _ID) = 0;

  virtual void SetTexture(const vdl::ID& _ID, ITexture* _pTexture) = 0;
};
