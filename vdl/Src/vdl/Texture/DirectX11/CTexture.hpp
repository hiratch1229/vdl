#pragma once
#include "../ITexture.hpp"

#include <vdl/Window.hpp>

#include <vdl/pch/DirectX11/pch.hpp>
#include <vdl/Platform/IPlatform.hpp>
#include <vdl/Constants/Constants.hpp>

struct CTexture : public ITexture
{
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
  vdl::uint2 TextureSize;
public:
  CTexture() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX11; }

  TextureType GetType()const override { return TextureType::eTexture; }

  vdl::uint2 GetSize()const final { return TextureSize; }

  vdl::FormatType GetFormat()const override { return Constants::kTextureFormat; }
};

struct CCubeTexture : public CTexture
{
public:
  CCubeTexture() = default;

  TextureType GetType()const override { return TextureType::eCubeTexture; }
};

struct CRenderTexture : public CTexture
{
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;
  vdl::FormatType Format;
public:
  CRenderTexture() = default;

  TextureType GetType()const final { return TextureType::eRenderTexture; }

  vdl::FormatType GetFormat()const final { return Format; }
};

struct CSwapChainRenderTexture : public ITexture
{
public:
  CSwapChainRenderTexture() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX11; }

  TextureType GetType()const final { return TextureType::eSwapChainRenderTexture; }

  vdl::uint2 GetSize()const final { return vdl::Window::GetWindowSize(); }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eSwapChain; }
};

struct CDepthStencilTexture : public IDepthStencilTexture
{
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
  vdl::uint2 TextureSize;
  vdl::Texture DepthTexture;
  vdl::Texture StencilTexture;
  vdl::FormatType Format;
public:
  CDepthStencilTexture() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX11; }

  TextureType GetType()const final { return TextureType::eDepthStencilTexture; }

  vdl::uint2 GetSize()const final { return TextureSize; }

  vdl::FormatType GetFormat()const final { return Format; }

  const vdl::Texture& GetDepthTexture()final;

  const vdl::Texture& GetStencilTexture()final;
};

struct CDepthTexture : public ITexture
{
  CDepthStencilTexture* pParent;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
public:
  CDepthTexture() = default;

  ~CDepthTexture();

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX11; }

  TextureType GetType()const final { return TextureType::eDepthTexture; }

  vdl::uint2 GetSize()const final { return pParent->TextureSize; }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eUnknown; }
};

struct CStencilTexture : public ITexture
{
  CDepthStencilTexture* pParent;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
public:
  CStencilTexture() = default;

  ~CStencilTexture();

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX11; }

  TextureType GetType()const final { return TextureType::eStencilTexture; }

  vdl::uint2 GetSize()const final { return pParent->TextureSize; }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eUnknown; }
};

struct CUnorderedAccessTexture : public CTexture
{
  Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pUnorderedAccessView;
  vdl::FormatType Format;
public:
  CUnorderedAccessTexture() = default;

  TextureType GetType()const final { return TextureType::eUnorderedAccessTexture; }

  vdl::FormatType GetFormat()const final { return Format; }
};

template<class Texture>
inline Texture* Cast(ITexture* _pTexture)
{
  static_assert(std::is_base_of<ITexture, Texture>::value);

  const TextureType Type = _pTexture->GetType();

  if constexpr (std::is_same<CTexture, Texture>::value)
  {
    assert(Type == TextureType::eTexture || Type == TextureType::eCubeTexture 
      || Type == TextureType::eRenderTexture || Type == TextureType::eUnorderedAccessTexture);
  }
  if constexpr (std::is_same<CCubeTexture, Texture>::value)
  {
    assert(Type == TextureType::eCubeTexture);
  }
  if constexpr (std::is_same<CRenderTexture, Texture>::value)
  {
    assert(Type == TextureType::eRenderTexture);
  }
  if constexpr (std::is_same<CSwapChainRenderTexture, Texture>::value)
  {
    assert(Type == TextureType::eSwapChainRenderTexture);
  }
  if constexpr (std::is_same<CDepthStencilTexture, Texture>::value)
  {
    assert(Type == TextureType::eDepthStencilTexture);
  }
  if constexpr (std::is_same<CDepthTexture, Texture>::value)
  {
    assert(Type == TextureType::eDepthTexture);
  }
  if constexpr (std::is_same<CStencilTexture, Texture>::value)
  {
    assert(Type == TextureType::eStencilTexture);
  }
  if constexpr (std::is_same<CUnorderedAccessTexture, Texture>::value)
  {
    assert(Type == TextureType::eUnorderedAccessTexture);
  }

  return Cast<Texture, ITexture>(_pTexture);
}
