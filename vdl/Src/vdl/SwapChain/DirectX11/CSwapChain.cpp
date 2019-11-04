#include "CSwapChain.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/Device/DirectX11/CDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vdl/Format/DirectX/Format.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/Window.hpp>
#include <vdl/Image.hpp>
#include <vdl/DetectMemoryLeak.hpp>

void CSwapChain::Initialize()
{
  pWindow_ = Engine::Get<IWindow>();
  pDeviceContext_ = Engine::Get<IDeviceContext>();

  CDevice* pDevice = static_cast<CDevice*>(Engine::Get<IDevice>());
  pD3D11Device_ = pDevice->GetDevice();
  pD3D11ImmediateContext_ = pDevice->GetImmediateContext();

  const HWND hWnd = static_cast<HWND>(pWindow_->GetHandle());

  constexpr DXGI_FORMAT kSwapChainFormat = Cast(vdl::FormatType::eSwapChain);
  constexpr DXGI_FORMAT kDepthStencilFormat = Cast(vdl::FormatType::eDepthStencil);

  //  エラーチェック用
  HRESULT hr = S_OK;

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

  //  ALT+ENTER無効にしてスワップチェーンを作成
  {
    Microsoft::WRL::ComPtr<IDXGIDevice> pDXGIDevice;
    hr = pD3D11Device_->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(pDXGIDevice.GetAddressOf()));
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));
    Microsoft::WRL::ComPtr<IDXGIAdapter> pAdapter;
    hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(pAdapter.GetAddressOf()));
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));
    Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;
    hr = pAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));

    hr = pFactory->CreateSwapChain(pD3D11Device_, &SwapChainDesc, pSwapChain_.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));

    hr = pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));
  }

  D3D11_TEXTURE2D_DESC BackBufferDesc;
  //  レンダーターゲットビューの作成
  {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    hr = pSwapChain_->GetBuffer(0, IID_PPV_ARGS(pBackBuffer.GetAddressOf()));
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));

    hr = pD3D11Device_->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pRenderTargetView_.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));

    pBackBuffer->GetDesc(&BackBufferDesc);
  }

  //  深度ステンシルビューの作成
  {
    D3D11_TEXTURE2D_DESC DepthStencilBufferDesc = std::move(BackBufferDesc);
    {
      DepthStencilBufferDesc.Format = kDepthStencilFormat;
      DepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilBuffer;
    {
      hr = pD3D11Device_->CreateTexture2D(&DepthStencilBufferDesc, nullptr, pDepthStencilBuffer.GetAddressOf());
      _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));
    }

    hr = pD3D11Device_->CreateDepthStencilView(pDepthStencilBuffer.Get(), nullptr, pDepthStencilView_.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));
  }
}

void CSwapChain::ScreenClear()
{
  pDeviceContext_->ClearRenderTexture(RenderTextures_[0], pWindow_->GetScreenClearColor());
  pDeviceContext_->ClearDepthStencilTexture(DepthStencilTexture_, Constants::kDefaultClearDepth, Constants::kDefaultClearStencil);
  pDeviceContext_->SetRenderTextures(RenderTextures_, DepthStencilTexture_);
}

void CSwapChain::Present()
{
  pSwapChain_->Present(0, 0);
}

void CSwapChain::ChangeWindowMode()
{
  //  TODO
}

void CSwapChain::ScreenShot()
{
  HRESULT hr = S_OK;

  Microsoft::WRL::ComPtr<ID3D11Texture2D> BackBuffer;
  {
    hr = pSwapChain_->GetBuffer(0, IID_PPV_ARGS(BackBuffer.GetAddressOf()));
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));
  }

  vdl::uint2 TextureSize;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> CopyBuffer;
  {
    D3D11_TEXTURE2D_DESC Texture2DDesc;
    {
      BackBuffer->GetDesc(&Texture2DDesc);
      Texture2DDesc.BindFlags = 0;
      Texture2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
      Texture2DDesc.Usage = D3D11_USAGE_STAGING;
    }

    TextureSize = { Texture2DDesc.Width, Texture2DDesc.Height };

    hr = pD3D11Device_->CreateTexture2D(&Texture2DDesc, nullptr, CopyBuffer.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));

    pD3D11ImmediateContext_->CopyResource(CopyBuffer.Get(), BackBuffer.Get());
  }

  vdl::Image Image;
  Image.Resize(TextureSize);
  {
    D3D11_MAPPED_SUBRESOURCE MappedSubresorce;
    hr = pD3D11ImmediateContext_->Map(CopyBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedSubresorce);
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    ::memcpy(Image.Buffer(), MappedSubresorce.pData, Image.BufferSize());

    pD3D11ImmediateContext_->Unmap(CopyBuffer.Get(), 0);
  }

  Image.SavePNG(Constants::kScreenShotFileDirectory);
}
