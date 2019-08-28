#pragma once
#include "../ITextureManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

class CTextureManager : public ITextureManager
{
  IDevice* pDevice_;
private:
  ReferenceCounts<ITexture> Textures_;
public:
  void Initialize(IDevice* _pDevice)override;

  ID Load(const char* _FilePath, bool _isSerialize)override;

  ID Load(const vdl::Image& _Image)override;

  ID CreateRenderTexture(const vdl::uint2& _TextureSize, vdl::Format _Format)override;

  ID CreateDepthStencilTexture(const vdl::uint2& _TextureSize, vdl::Format _Format)override;

  ITexture* GetTexture(const ID& _ID) { return Textures_.Get(_ID).Get(); }
};
