#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Format.hpp>
#include <vdl/ID.hpp>

#include <vdl/Texture/ITexture.hpp>

class ITextureManager
{
public:
  static ITextureManager* Create();

  ITextureManager() = default;

  virtual ~ITextureManager() = default;

  virtual void Initialize() = 0;

  virtual ID Load(const char* _FilePath) = 0;

  virtual ID CreateRenderTexture(const vdl::uint2& _Size, vdl::Format _Format) = 0;

  virtual ID CreateDepthStencilTexture(const vdl::uint2& _Size, vdl::Format _Format) = 0;

  virtual ITexture* GetTexture(const ID& _ID) = 0;
};
