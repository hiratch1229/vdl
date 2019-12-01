#pragma once
#include "../ITexture.hpp"

#include <vdl/pch/DirectX12/pch.hpp>

#include <vdl/Window.hpp>

#include <vdl/Constants/Constants.hpp>

struct TextureData
{
  Microsoft::WRL::ComPtr<ID3D12Resource> pResource;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pShaderResourceViewHeap;
  D3D12_RESOURCE_STATES ResourceState = D3D12_RESOURCE_STATE_COMMON;
public:
  TextureData() = default;

  TextureData(TextureData&& _Data)noexcept
    : pResource(std::move(_Data.pResource)), pShaderResourceViewHeap(std::move(_Data.pShaderResourceViewHeap))
    , ResourceState(std::move(_Data.ResourceState)){}

  TextureData& operator=(TextureData&& _Data)noexcept
  {
    pResource = std::move(_Data.pResource);
    pShaderResourceViewHeap = std::move(_Data.pShaderResourceViewHeap);
    ResourceState = std::move(_Data.ResourceState);

    return *this;
  }

  void TransitionResourceBarrier(ID3D12GraphicsCommandList* _pCommandList, D3D12_RESOURCE_STATES _AfterState);
};

struct CTexture : public ITexture
{
  TextureData TextureData;
  vdl::uint2 TextureSize;
public:
  CTexture() = default;

  TextureType GetType()const override { return TextureType::eTexture; }

  vdl::uint2 GetSize()const final { return TextureSize; }

  vdl::FormatType GetFormat()const override { return Constants::kTextureFormat; }
public:
  static void TransitionResourceBarrier(ID3D12GraphicsCommandList* _pCommandList, ID3D12Resource* _pResource,
    D3D12_RESOURCE_STATES _BeforeState, D3D12_RESOURCE_STATES _AfterState);
};

struct CCubeTexture : public CTexture
{
public:
  CCubeTexture() = default;

  TextureType GetType()const final { return TextureType::eCubeTexture; }
};

struct CRenderTexture : public CTexture
{
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pRenderTargetViewHeap;
  DXGI_FORMAT DXFormat;
  vdl::FormatType Format;
public:
  CRenderTexture() = default;

  TextureType GetType()const override { return TextureType::eRenderTexture; }

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
  TextureData TextureData;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pDepthStencilViewHeap;
  vdl::uint2 TextureSize;
  vdl::Texture DepthTexture;
  vdl::Texture StencilTexture;
  DXGI_FORMAT DXFormat;
  vdl::FormatType Format;
public:
  CDepthStencilTexture() = default;

  TextureType GetType()const override { return TextureType::eDepthStencilTexture; }

  vdl::uint2 GetSize()const final { return TextureSize; }

  vdl::FormatType GetFormat()const final { return Format; }

  const vdl::Texture& GetDepthTexture()final;

  const vdl::Texture& GetStencilTexture()final;
};

struct CDepthTexture : public ITexture
{
  CDepthStencilTexture* pParent;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pShaderResourceViewHeap;
public:
  CDepthTexture() = default;

  ~CDepthTexture();

  TextureType GetType()const final { return TextureType::eDepthTexture; }

  vdl::uint2 GetSize()const final { return pParent->GetSize(); }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eUnknown; }
};

struct CStencilTexture : public ITexture
{
  CDepthStencilTexture* pParent;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pShaderResourceViewHeap;
public:
  CStencilTexture() = default;

  ~CStencilTexture();

  TextureType GetType()const final { return TextureType::eStencilTexture; }

  vdl::uint2 GetSize()const final { return pParent->GetSize(); }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eUnknown; }
};

struct CUnorderedAccessTexture : public CTexture
{
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pUnorededAccessViewHeap;
  vdl::FormatType Format;
public:
  CUnorderedAccessTexture() = default;

  TextureType GetType()const override { return TextureType::eUnorderedAccessTexture; }

  vdl::FormatType GetFormat()const final { return Format; }
};

template<class Texture>
inline Texture* Cast(ITexture* _pTexture)
{
  static_assert(std::is_base_of<ITexture, Texture>::value);

  if constexpr (std::is_same<CTexture, Texture>::value)
  {
    assert(_pTexture->GetType() == TextureType::eTexture || _pTexture->GetType() == TextureType::eCubeTexture || _pTexture->GetType() == TextureType::eRenderTexture || _pTexture->GetType() == TextureType::eDepthTexture
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
