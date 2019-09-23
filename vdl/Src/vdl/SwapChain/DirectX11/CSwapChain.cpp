#include "CSwapChain.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vdl/Format/DirectX/Format.hpp>
#include <vdl/Device/DirectX11/CDevice.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

void CSwapChain::Initialize()
{
  pWindow_ = Engine::Get<IWindow>();

  const HWND hWnd = static_cast<HWND>(pWindow_->GetHandle());

  constexpr DXGI_FORMAT kDepthStencilFormat = Cast(Constants::kDepthStencilFormat);

  //  エラーチェック用
  HRESULT hr = S_OK;

  DXGI_SWAP_CHAIN_DESC SwapChainDesc;
  {
    SwapChainDesc.BufferDesc.Width = Constants::kDefaultWindowSize.x;
    SwapChainDesc.BufferDesc.Height = Constants::kDefaultWindowSize.y;
    SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    SwapChainDesc.BufferDesc.Format = Cast(Constants::kSwapChainFormat);
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

  ID3D11Device* pDevice = static_cast<CDevice*>(Engine::Get<IDevice>())->GetDevice();
  //  ALT+ENTER無効にしてスワップチェーンを作成
  {
    Microsoft::WRL::ComPtr<IDXGIDevice> pDXGIDevice;
    hr = pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(pDXGIDevice.GetAddressOf()));
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));
    Microsoft::WRL::ComPtr<IDXGIAdapter> pAdapter;
    hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(pAdapter.GetAddressOf()));
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));
    Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;
    hr = pAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));

    hr = pFactory->CreateSwapChain(pDevice, &SwapChainDesc, pSwapChain_.GetAddressOf());
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

    hr = pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pRenderTargetView_.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));

    pBackBuffer->GetDesc(&BackBufferDesc);
  }

  //  深度ステンシルビューの作成
  {
    D3D11_TEXTURE2D_DESC DepthStencilBufferDesc = BackBufferDesc;
    {
      DepthStencilBufferDesc.Format = kDepthStencilFormat;
      DepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilBuffer;
    {
      hr = pDevice->CreateTexture2D(&DepthStencilBufferDesc, nullptr, pDepthStencilBuffer.GetAddressOf());
      _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
    {
      DepthStencilViewDesc.Format = kDepthStencilFormat;
      DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
      DepthStencilViewDesc.Flags = 0;
      DepthStencilViewDesc.Texture2D.MipSlice = 0;
    }

    hr = pDevice->CreateDepthStencilView(pDepthStencilBuffer.Get(), &DepthStencilViewDesc, pDepthStencilView_.GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hResultTrace(hr));
  }
}

void CSwapChain::ScreenClear()
{
  pDeviceContext_->ClearRenderTexture(RenderTexture_, pWindow_->GetScreenClearColor());
  pDeviceContext_->ClearDepthStencilTexture(DepthStencilTexture_, Constants::kDefaultClearDepth, Constants::kDefaultClearStencil);
  pDeviceContext_->SetRenderTexture(RenderTexture_, DepthStencilTexture_);
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
  //  TODO
}
