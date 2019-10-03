#pragma once
#include "../ITexture.hpp"

#include <d3d11.h>
#include <wrl.h>
#include <dxgiformat.h>

struct CTexture : public ITexture
{
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
  vdl::uint2 TextureSize;
public:
  CTexture() = default;

  TextureType GetType()const { return TextureType::eTexture; }

  vdl::uint2 GetSize()const final { return TextureSize; }
};

struct CRenderTexture : public CTexture
{
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;
public:
  CRenderTexture() = default;

  TextureType GetType()const final { return TextureType::eRenderTexture; }
};

struct CDepthStencilTexture : public CTexture
{
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
public:
  CDepthStencilTexture() = default;

  TextureType GetType()const { return TextureType::eDepthStencilTexture; }
};

struct CUnorderedAccessTexture : public CTexture
{
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
  Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pUnorderedAccessView;
public:
  CUnorderedAccessTexture() = default;

  TextureType GetType()const { return TextureType::eUnorderedAccessTexture; }
};
