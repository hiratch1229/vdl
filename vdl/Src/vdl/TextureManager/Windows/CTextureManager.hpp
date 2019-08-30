#pragma once
#include "../ITextureManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

class IDevice;

class CTextureManager : public ITextureManager
{
  IDevice* pDevice_;
private:
  ReferenceCounts<ITexture> Textures_;
public:
  void Initialize()override;

  ID Load(const char* _FilePath, bool _isSerialize)override;

  ID Load(const vdl::Image& _Image)override;

  ID CreateRenderTexture(const vdl::uint2& _TextureSize, vdl::Format _Format)override;

  ID CreateDepthStencilTexture(const vdl::uint2& _TextureSize, vdl::Format _Format)override;

  void AddRef(const ID& _ID)override { Textures_.Get(_ID).AddRef(); }

  void Release(const ID& _ID)override { Textures_.Get(_ID).Release(); }

  ITexture* GetTexture(const ID& _ID) { return Textures_.Get(_ID).Get(); }
};
