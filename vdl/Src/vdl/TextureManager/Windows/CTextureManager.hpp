#pragma once
#include "../ITextureManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

class IDevice;

class CTextureManager : public ITextureManager
{
  IDevice* pDevice_;
  ReferenceCounts<ITexture> Textures_;
public:
  void Initialize()override;

  vdl::ID Load(const char* _FilePath, bool _isSerialize)override;

  vdl::ID Load(const vdl::Image& _Image)override;

  vdl::ID CreateRenderTexture(const vdl::uint2& _TextureSize, vdl::Format _Format)override;

  vdl::ID CreateDepthStencilTexture(const vdl::uint2& _TextureSize, vdl::Format _Format)override;

  void AddRef(const vdl::ID& _ID)override { Textures_.Get(_ID).AddRef(); }

  void Release(const vdl::ID& _ID)override { Textures_.Get(_ID).Release(); }

  ITexture* GetTexture(const vdl::ID& _ID) { return Textures_.Get(_ID).Get(); }
};
