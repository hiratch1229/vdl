#pragma once
#include "../ITexture.hpp"

#include <vdl/pch/DirectX11/pch.hpp>

#include <vdl/Window.hpp>

#include <vdl/Constants/Constants.hpp>

struct CTexture : public ITexture
{
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
  vdl::uint2 TextureSize;
public:
  CTexture() = default;

  TextureType GetType()const override { return TextureType::eTexture; }

  vdl::uint2 GetSize()const final { return TextureSize; }

  vdl::FormatType GetFormat()const override { return Constants::kTextureFormat; }
};

struct CCubeTexture : public ITexture
{
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
  vdl::uint2 TextureSize;
public:
  CCubeTexture() = default;

  TextureType GetType()const override { return TextureType::eCubeTexture; }

  vdl::uint2 GetSize()const final { return TextureSize; }

  vdl::FormatType GetFormat()const override { return Constants::kTextureFormat; }
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

  TextureType GetType()const final { return TextureType::eSwapChainRenderTexture; }

  vdl::uint2 GetSize()const final { return vdl::Window::GetWindowSize(); }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eSwapChain; }
};

struct CDepthStencilTexture : public IDepthStencilTexture
{
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
  vdl::Texture DepthTexture;
  vdl::Texture StencilTexture;
  vdl::FormatType Format;
public:
  CDepthStencilTexture() = default;

  TextureType GetType()const final { return TextureType::eDepthStencilTexture; }

  vdl::uint2 GetSize()const final { return DepthTexture.GetSize(); }

  vdl::FormatType GetFormat()const final { return Format; }

  const vdl::Texture& GetDepthTexture()final { return DepthTexture; }

  const vdl::Texture& GetStencilTexture()final { return StencilTexture; }
};

struct CDepthTexture : public CTexture
{
public:
  CDepthTexture() = default;

  TextureType GetType()const final { return TextureType::eDepthTexture; }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eUnknown; }
};

struct CStencilTexture : public CTexture
{
public:
  CStencilTexture() = default;

  TextureType GetType()const final { return TextureType::eStencilTexture; }

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

  if constexpr (std::is_same<CTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eTexture || _pTexture->GetType() == TextureType::eRenderTexture || _pTexture->GetType() == TextureType::eDepthTexture
      || _pTexture->GetType() == TextureType::eStencilTexture || _pTexture->GetType() == TextureType::eUnorderedAccessTexture);
  }
  if constexpr (std::is_same<CCubeTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eCubeTexture);
  }
  if constexpr (std::is_same<CRenderTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eRenderTexture);
  }
  if constexpr (std::is_same<CSwapChainRenderTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eSwapChainRenderTexture);
  }
  if constexpr (std::is_same<CDepthStencilTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eDepthStencilTexture);
  }
  if constexpr (std::is_same<CDepthTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eDepthTexture);
  }
  if constexpr (std::is_same<CStencilTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eStencilTexture);
  }
  if constexpr (std::is_same<CUnorderedAccessTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eUnorderedAccessTexture);
  }

  return static_cast<Texture*>(_pTexture);
}
