#pragma once
#include "../ISwapChain.hpp"

#include <vdl/Texture.hpp>

#include <vdl/pch/DirectX11/pch.hpp>

class IWindow;
class IDeviceContext;

class CSwapChain : public ISwapChain
{
  Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain_;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView_;
private:
  ID3D11Device* pD3D11Device_;
  ID3D11DeviceContext* pD3D11ImmediateContext_;
private:
  IWindow* pWindow_;
  IDeviceContext* pDeviceContext_;
private:
  vdl::RenderTextures RenderTextures_;
  vdl::DepthStencilTexture DepthStencilTexture_;
public:
  ID3D11RenderTargetView* GetRenderTargetView()const { return pRenderTargetView_.Get(); }
public:
  CSwapChain() = default;

  void Initialize()override;

  void ScreenClear()override;

  void Present()override;

  void ChangeWindowMode()override;

  void ScreenShot()override;

  const vdl::RenderTexture& GetRenderTexture()const override { return RenderTextures_[0]; }

  const vdl::DepthStencilTexture& GetDepthStencilTexture()const override { return DepthStencilTexture_; }
};
