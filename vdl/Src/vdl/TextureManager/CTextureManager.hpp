#pragma once
#include "ITextureManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

class IDevice;

class CTextureManager : public ITextureManager
{
  IDevice* pDevice_;
  ReferenceCounts<ITexture> Textures_;
private:
  vdl::Image GetImageFromFilePath(const char* _FilePath)const;
public:
  CTextureManager() = default;

  PlatformType GetPlatform()const final { return PlatformType::eCommon; }

  void Initialize()override;

  vdl::ID LoadTexture(const char* _FilePath)override;

  vdl::ID LoadTexture(const vdl::Image& _Image)override;

  vdl::ID LoadCubeTexture(const char* _FilePath)override;

  vdl::ID LoadCubeTexture(const vdl::Image& _Image)override;

  vdl::ID CreateRenderTexture(const vdl::uint2& _TextureSize, vdl::FormatType _Format)override;

  vdl::ID CreateDepthStencilTexture(const vdl::uint2& _TextureSize, vdl::FormatType _Format)override;

  vdl::ID CreateUnorderedAccessTexture(const vdl::uint2& _TextureSize, vdl::FormatType _Format)override;

  void AddRef(const vdl::ID& _ID)override { Textures_.Get(_ID).AddRef(); }

  void Release(const vdl::ID& _ID)override { Textures_.Get(_ID).Release(); }

  ITexture* GetTexture(const vdl::ID& _ID) { return Textures_.Get(_ID).Get(); }

  void SetTexture(const vdl::ID& _ID, ITexture* _pTexture)override { Textures_.Get(_ID) = _pTexture; }
};
