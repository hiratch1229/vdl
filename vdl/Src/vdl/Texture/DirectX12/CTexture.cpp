#include "CTexture.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/DirectX12/CDevice.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>

#include <vdl/Format/Format.hpp>
#include <vdl/Format/DirectX/Format.hpp>

#include <vdl/Misc/Windows/Misc.hpp>

void CTexture::TransitionResourceBarrier(ID3D12GraphicsCommandList* _pCommandList, ID3D12Resource* _pResource,
  D3D12_RESOURCE_STATES _BeforeState, D3D12_RESOURCE_STATES _AfterState)
{
  D3D12_RESOURCE_BARRIER ResourceBarrier;
  {
    ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    ResourceBarrier.Transition.pResource = _pResource;
    ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    ResourceBarrier.Transition.StateBefore = _BeforeState;
    ResourceBarrier.Transition.StateAfter = _AfterState;
  }

  _pCommandList->ResourceBarrier(1, &ResourceBarrier);
}

//--------------------------------------------------

void TextureData::TransitionResourceBarrier(ID3D12GraphicsCommandList* _pCommandList, D3D12_RESOURCE_STATES _AfterState)
{
  CTexture::TransitionResourceBarrier(_pCommandList, pResource.Get(), ResourceState, _AfterState);
  ResourceState = _AfterState;
}

//--------------------------------------------------

const vdl::Texture& CDepthStencilTexture::GetDepthTexture()
{
  if (DepthTexture.isEmpty())
  {
    DepthTexture = vdl::Palette::White;

    CDepthTexture* pDepthTexture = new CDepthTexture;
    pDepthTexture->pParent = this;

    HRESULT hr = S_OK;

    ID3D12Device5* pD3D12Device = static_cast<CDevice*>(Engine::Get<IDevice>())->GetDevice();

    //  ディスクリプタヒープの作成
    {
      D3D12_DESCRIPTOR_HEAP_DESC ShaderResourceViewHeapDesc;
      {
        ShaderResourceViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        ShaderResourceViewHeapDesc.NumDescriptors = 1;
        ShaderResourceViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ShaderResourceViewHeapDesc.NodeMask = 0;
      }

      hr = pD3D12Device->CreateDescriptorHeap(&ShaderResourceViewHeapDesc, IID_PPV_ARGS(pDepthTexture->pShaderResourceViewHeap.GetAddressOf()));
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    //  ビューの作成
    {
      D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
      {
        ShaderResourceViewDesc.Format = DepthFormatFromDepthStencilFormat(Format);
        ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        ShaderResourceViewDesc.Texture2D.MipLevels = 1;
        ShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
        ShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
      }
      pD3D12Device->CreateShaderResourceView(TextureData.pResource.Get(), &ShaderResourceViewDesc, pDepthTexture->pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart());
    }

    ITextureManager* pTextureManager = Engine::Get<ITextureManager>();
    pTextureManager->SetTexture(DepthTexture.GetID(), pDepthTexture);

    //  TODO:親のオブジェクトを渡して参照カウントを増やす
    for (vdl::uint i = 0;; ++i)
    {
      if (pDepthTexture->pParent == pTextureManager->GetTexture(i))
      {
        pTextureManager->AddRef(i);
        break;
      }
    }
  }

  return DepthTexture;
}

const vdl::Texture& CDepthStencilTexture::GetStencilTexture()
{
  if (hasStencil(Format) && StencilTexture.isEmpty())
  {
    StencilTexture = vdl::Palette::White;

    CStencilTexture* pStencilTexture = new CStencilTexture;
    pStencilTexture->pParent = this;

    HRESULT hr = S_OK;

    ID3D12Device5* pD3D12Device = static_cast<CDevice*>(Engine::Get<IDevice>())->GetDevice();

    //  ディスクリプタヒープの作成
    {
      D3D12_DESCRIPTOR_HEAP_DESC ShaderResourceViewHeapDesc;
      {
        ShaderResourceViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        ShaderResourceViewHeapDesc.NumDescriptors = 1;
        ShaderResourceViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ShaderResourceViewHeapDesc.NodeMask = 0;
      }

      hr = pD3D12Device->CreateDescriptorHeap(&ShaderResourceViewHeapDesc, IID_PPV_ARGS(pStencilTexture->pShaderResourceViewHeap.GetAddressOf()));
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    //  ビューの作成
    {
      D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
      {
        ShaderResourceViewDesc.Format = DepthFormatFromDepthStencilFormat(Format);
        ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        ShaderResourceViewDesc.Texture2D.MipLevels = 1;
        ShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
        ShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
      }
     
      pD3D12Device->CreateShaderResourceView(TextureData.pResource.Get(), &ShaderResourceViewDesc, pStencilTexture->pShaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart());
    }

    ITextureManager* pTextureManager = Engine::Get<ITextureManager>();
    pTextureManager->SetTexture(StencilTexture.GetID(), pStencilTexture);

    //  TODO:親のオブジェクトを渡して参照カウントを増やす
    for (vdl::uint i = 0;; ++i)
    {
      if (pStencilTexture->pParent == pTextureManager->GetTexture(i))
      {
        pTextureManager->AddRef(i);
        break;
      }
    }
  }

  return StencilTexture;
}

//--------------------------------------------------

CDepthTexture::~CDepthTexture()
{
  assert(pParent);

  ITextureManager* pTextureManager = Engine::Get<ITextureManager>();
  for (vdl::uint i = 0;; ++i)
  {
    if (pParent == pTextureManager->GetTexture(i))
    {
      pTextureManager->Release(i);
      break;
    }
  }
}

//--------------------------------------------------

CStencilTexture::~CStencilTexture()
{
  assert(pParent);

  ITextureManager* pTextureManager = Engine::Get<ITextureManager>();
  for (vdl::uint i = 0;; ++i)
  {
    if (pParent == pTextureManager->GetTexture(i))
    {
      pTextureManager->Release(i);
      break;
    }
  }
}
