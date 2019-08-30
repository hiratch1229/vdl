#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/ID.hpp>

#include <vdl/Texture/ITexture.hpp>

class ITextureManager
{
public:
  static ITextureManager* Create();

  ITextureManager() = default;

  virtual ~ITextureManager() = default;

  virtual void Initialize() = 0;

  virtual ID Load(const char* _FilePath, bool _isSerialize) = 0;

  virtual ID Load(const vdl::Image& _Image) = 0;

  virtual ID CreateRenderTexture(const vdl::uint2& _TextureSize, vdl::Format _Format) = 0;

  virtual ID CreateDepthStencilTexture(const vdl::uint2& _TextureSize, vdl::Format _Format) = 0;

  virtual void AddRef(const ID& _ID) = 0;

  virtual void Release(const ID& _ID) = 0;

  virtual ITexture* GetTexture(const ID& _ID) = 0;
};
