#include "CSwapChain.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/Device/DirectX12/CDevice.hpp>
#include <vdl/DeviceContext/DirectX12/CDeviceContext.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>

#include <vdl/Format/DirectX/Format.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

void CSwapChain::Initialize()
{
  pWindow_ = Engine::Get<IWindow>();
  pDeviceContext_ = static_cast<CDeviceContext*>(Engine::Get<IDeviceContext>());

  CDevice* pDevice = static_cast<CDevice*>(Engine::Get<IDevice>());
  ID3D12Device5* pD3D12Device = pDevice->GetDevice();
  IDXGIFactory6* pFactory = pDevice->GetFactory();

  const HWND hWnd = static_cast<HWND>(pWindow_->GetHandle());
  constexpr DXGI_FORMAT kSwapChainFormat = Cast(vdl::FormatType::eSwapChain);

  //  �G���[�`�F�b�N�p
  HRESULT hr = S_OK;

  //  �X���b�v�`�F�[���̍쐬
  {
    DXGI_SWAP_CHAIN_DESC SwapChainDesc;
    {
      SwapChainDesc.BufferDesc.Width = Constants::kDefaultWindowSize.x;
      SwapChainDesc.BufferDesc.Height = Constants::kDefaultWindowSize.y;
      SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
      SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
      SwapChainDesc.BufferDesc.Format = kSwapChainFormat;
      SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
      SwapChainDesc.SampleDesc.Count = 1;
      SwapChainDesc.SampleDesc.Quality = 0;
      SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      SwapChainDesc.BufferCount = Constants::kBackBufferNum;
      SwapChainDesc.OutputWindow = hWnd;
      SwapChainDesc.Windowed = true;
      SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
      SwapChainDesc.Flags = 0;
    }

    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
    hr = pFactory->CreateSwapChain(pDeviceContext_->GetGraphicsCommandQueue(), &SwapChainDesc, pSwapChain.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));

    hr = pSwapChain->QueryInterface(IID_PPV_ARGS(pSwapChain_.GetAddressOf()));
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    //  ALT+Enter�̖�����
    hr = pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));
  }

  //  �o�b�N�o�b�t�@�̍쐬
  {
    //  �o�b�N�o�b�t�@�̃C���f�b�N�X���擾
    CurrentBufferIndex_ = pSwapChain_->GetCurrentBackBufferIndex();

    D3D12_DESCRIPTOR_HEAP_DESC RenderTargetDescriptorHeapDesc;
    {
      RenderTargetDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
      RenderTargetDescriptorHeapDesc.NumDescriptors = 1;
      RenderTargetDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
      RenderTargetDescriptorHeapDesc.NodeMask = 0;
    }

    const vdl::uint RenderTargetViewDescriptorSize = pD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12RenderTextures_.resize(Constants::kBackBufferNum);
    for (vdl::uint i = 0; i < Constants::kBackBufferNum; ++i)
    {
      CRenderTexture& D3D12RenderTexture = D3D12RenderTextures_[i];

      D3D12RenderTexture.TextureSize = Constants::kDefaultWindowSize;
      D3D12RenderTexture.DXFormat = kSwapChainFormat;
      D3D12RenderTexture.Format = vdl::FormatType::eSwapChain;
      //D3D12RenderTexture.TextureData.ResourceState = D3D12_RESOURCE_STATE_PRESENT;

      //  �����_�[�^�[�Q�b�g�̃f�X�N���v�^�q�[�v�쐬
      {
        hr = pD3D12Device->CreateDescriptorHeap(&RenderTargetDescriptorHeapDesc, IID_PPV_ARGS(D3D12RenderTexture.pRenderTargetViewHeap.GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
      }

      //  �����_�[�^�[�Q�b�g�r���[�̍쐬
      {
        hr = pSwapChain_->GetBuffer(i, IID_PPV_ARGS(D3D12RenderTexture.TextureData.pResource.GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

        pD3D12Device->CreateRenderTargetView(D3D12RenderTexture.TextureData.pResource.Get(), nullptr, D3D12RenderTexture.pRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());
      }
    }
  }

  //  �����_�[�e�N�X�`���̍쐬
  {
    RenderTextures_[0] = vdl::RenderTexture(Constants::kDefaultWindowSize, vdl::FormatType::eSwapChain);

    CSwapChainRenderTexture* pRenderTexture = new CSwapChainRenderTexture;

    Engine::Get<ITextureManager>()->SetTexture(RenderTextures_[0].GetID(), pRenderTexture);
  }

  //  �[�x�X�e���V���e�N�X�`���̍쐬
  DepthStencilTexture_ = vdl::DepthStencilTexture(Constants::kDefaultWindowSize, vdl::FormatType::eDefaultDepthStencil);
}

void CSwapChain::ScreenClear()
{
  pDeviceContext_->ClearRenderTexture(RenderTextures_[0], pWindow_->GetScreenClearColor());
  pDeviceContext_->ClearDepthStencilTexture(DepthStencilTexture_, Constants::kDefaultClearDepth, Constants::kDefaultClearStencil);
  pDeviceContext_->SetRenderTextures(RenderTextures_, DepthStencilTexture_);
}

void CSwapChain::Present()
{
  pDeviceContext_->Present();

  ++CurrentBufferIndex_ %= Constants::kBackBufferNum;
}

void CSwapChain::ChangeWindowMode()
{
  //  TODO
}

void CSwapChain::ScreenShot()
{
  //  TODO
}
