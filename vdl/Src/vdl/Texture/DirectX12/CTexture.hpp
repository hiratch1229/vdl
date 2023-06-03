#pragma once
#include "../ITexture.hpp"

#include <vdl/Window.hpp>

#include <vdl/Platform/IPlatform.hpp>
#include <vdl/Constants/Constants.hpp>
#include <vdl/Device/DirectX12/DescriptorHeap/DescriptorHeap.hpp>

#include <vdl/pch/DirectX12/pch.hpp>

#include <utility>

class CommandList;

struct TextureData
{
  Microsoft::WRL::ComPtr<ID3D12Resource> pResource;
  D3D12_RESOURCE_STATES ResourceState;
public:
  TextureData() = default;

  TextureData(TextureData&& _Data)noexcept
    : pResource(std::move(_Data.pResource)), ResourceState(std::move(_Data.ResourceState)) {}

  TextureData& operator=(TextureData&& _Data)noexcept
  {
    pResource = std::move(_Data.pResource);
    ResourceState = std::move(_Data.ResourceState);

    return *this;
  }

  void TransitionResourceBarrier(CommandList* _pCommandList, D3D12_RESOURCE_STATES _AfterState);
};

struct CTexture : public ITexture
{
  TextureData TextureData;
  vdl::uint2 TextureSize;
  DescriptorHeap ShaderResourceViewHeap;
public:
  CTexture() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

  TextureType GetType()const override { return TextureType::eTexture; }

  vdl::uint2 GetSize()const final { return TextureSize; }

  vdl::FormatType GetFormat()const override { return Constants::kTextureFormat; }
public:
  static void TransitionResourceBarrier(CommandList* _pCommandList, ID3D12Resource* _pResource,
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
  DescriptorHeap RenderTargetViewHeap;
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

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

  TextureType GetType()const final { return TextureType::eSwapChainRenderTexture; }

  vdl::uint2 GetSize()const final { return vdl::Window::GetWindowSize(); }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eSwapChain; }
};

struct CDepthStencilTexture : public IDepthStencilTexture
{
  TextureData TextureData;
  DescriptorHeap DepthStencilViewHeap;
  vdl::uint2 TextureSize;
  vdl::Texture DepthTexture;
  vdl::Texture StencilTexture;
  DXGI_FORMAT DXFormat;
  vdl::FormatType Format;
public:
  CDepthStencilTexture() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

  TextureType GetType()const override { return TextureType::eDepthStencilTexture; }

  vdl::uint2 GetSize()const final { return TextureSize; }

  vdl::FormatType GetFormat()const final { return Format; }

  const vdl::Texture& GetDepthTexture()final;

  const vdl::Texture& GetStencilTexture()final;
};

struct CDepthTexture : public ITexture
{
  CDepthStencilTexture* pParent;
  DescriptorHeap ShaderResourceViewHeap;
public:
  CDepthTexture() = default;

  ~CDepthTexture();

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

  TextureType GetType()const final { return TextureType::eDepthTexture; }

  vdl::uint2 GetSize()const final { return pParent->GetSize(); }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eUnknown; }
};

struct CStencilTexture : public ITexture
{
  CDepthStencilTexture* pParent;
  DescriptorHeap ShaderResourceViewHeap;
public:
  CStencilTexture() = default;

  ~CStencilTexture();

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

  TextureType GetType()const final { return TextureType::eStencilTexture; }

  vdl::uint2 GetSize()const final { return pParent->GetSize(); }

  vdl::FormatType GetFormat()const final { return vdl::FormatType::eUnknown; }
};

struct CUnorderedAccessTexture : public CTexture
{
  DescriptorHeap UnorderedAccessViewHeap;
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

  const TextureType Type = _pTexture->GetType();

  if constexpr (std::is_same<CTexture, Texture>::value)
  {
    assert(Type == TextureType::eTexture || Type == TextureType::eCubeTexture
      || Type == TextureType::eRenderTexture || Type == TextureType::eUnorderedAccessTexture);
  }
  else if constexpr (std::is_same<CCubeTexture, Texture>::value)
  {
    assert(Type == TextureType::eCubeTexture);
  }
  else if constexpr (std::is_same<CRenderTexture, Texture>::value)
  {
    assert(Type == TextureType::eRenderTexture);
  }
  else if constexpr (std::is_same<CSwapChainRenderTexture, Texture>::value)
  {
    assert(Type == TextureType::eSwapChainRenderTexture);
  }
  else if constexpr (std::is_same<CDepthStencilTexture, Texture>::value)
  {
    assert(Type == TextureType::eDepthStencilTexture);
  }
  else if constexpr (std::is_same<CDepthTexture, Texture>::value)
  {
    assert(Type == TextureType::eDepthTexture);
  }
  else if constexpr (std::is_same<CStencilTexture, Texture>::value)
  {
    assert(Type == TextureType::eStencilTexture);
  }
  else if constexpr (std::is_same<CUnorderedAccessTexture, Texture>::value)
  {
    assert(Type == TextureType::eUnorderedAccessTexture);
  }

  return Cast<Texture, ITexture>(_pTexture);
}
