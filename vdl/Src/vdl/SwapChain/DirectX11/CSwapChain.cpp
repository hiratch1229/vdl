#include "CSwapChain.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/Device/DirectX11/CDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>
#include <vdl/Renderer/IRenderer.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>
#include <vdl/Texture/DirectX11/CTexture.hpp>

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
  pRenderer_ = Engine::Get<IRenderer>();

  CDevice* pDevice = Cast<CDevice>(Engine::Get<IDevice>());
  pD3D11Device_ = pDevice->GetDevice();
  pD3D11ImmediateContext_ = pDevice->GetImmediateContext();
  pSwapChain_ = pDevice->GetSwapChain();

  //  エラーチェック用
  HRESULT hr = S_OK;

  //  バックバッファの作成
  {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    hr = pSwapChain_->GetBuffer(0, IID_PPV_ARGS(pBackBuffer.GetAddressOf()));
    ERROR_CHECK(hr);

    hr = pD3D11Device_->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pRenderTargetView_.GetAddressOf());
    ERROR_CHECK(hr);
  }

  //  レンダーテクスチャの作成
  {
    RenderTextures_[0] = vdl::RenderTexture(Constants::kDefaultWindowSize, vdl::FormatType::eSwapChain);

    CSwapChainRenderTexture* pRenderTexture = new CSwapChainRenderTexture;

    Engine::Get<ITextureManager>()->SetTexture(RenderTextures_[0].GetID(), pRenderTexture);
  }

  //  深度ステンシルテクスチャの作成
  DepthStencilTexture_ = vdl::DepthStencilTexture(Constants::kDefaultWindowSize, vdl::FormatType::eDefaultDepthStencil);
}

void CSwapChain::ScreenClear()
{
  pDeviceContext_->ClearRenderTexture(RenderTextures_[0], pWindow_->GetScreenClearColor());
  pDeviceContext_->ClearDepthStencilTexture(DepthStencilTexture_, Constants::kDefaultClearDepth, Constants::kDefaultClearStencil);
  pRenderer_->SetRenderTextures(RenderTextures_, DepthStencilTexture_);
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
  //HRESULT hr = S_OK;
  //
  //Microsoft::WRL::ComPtr<ID3D11Texture2D> BackBuffer;
  //{
  //  hr = pSwapChain_->GetBuffer(0, IID_PPV_ARGS(BackBuffer.GetAddressOf()));
  //  ERROR_CHECK(hr);
  //}
  //
  //vdl::uint2 TextureSize;
  //Microsoft::WRL::ComPtr<ID3D11Texture2D> CopyBuffer;
  //{
  //  D3D11_TEXTURE2D_DESC Texture2DDesc;
  //  {
  //    BackBuffer->GetDesc(&Texture2DDesc);
  //    Texture2DDesc.BindFlags = 0;
  //    Texture2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
  //    Texture2DDesc.Usage = D3D11_USAGE_STAGING;
  //  }
  //
  //  TextureSize = { Texture2DDesc.Width, Texture2DDesc.Height };
  //
  //  hr = pD3D11Device_->CreateTexture2D(&Texture2DDesc, nullptr, CopyBuffer.GetAddressOf());
  //  ERROR_CHECK(hr);
  //
  //  pD3D11ImmediateContext_->CopyResource(CopyBuffer.Get(), BackBuffer.Get());
  //}
  //
  //vdl::Image Image;
  //Image.Resize(TextureSize);
  //{
  //  D3D11_MAPPED_SUBRESOURCE MappedSubresorce;
  //  hr = pD3D11ImmediateContext_->Map(CopyBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedSubresorce);
  //  ERROR_CHECK(hr);
  //
  //  ::memcpy(Image.Buffer(), MappedSubresorce.pData, Image.BufferSize());
  //
  //  pD3D11ImmediateContext_->Unmap(CopyBuffer.Get(), 0);
  //}
  //
  //Image.SavePNG(Constants::kScreenShotFileDirectory);
}
