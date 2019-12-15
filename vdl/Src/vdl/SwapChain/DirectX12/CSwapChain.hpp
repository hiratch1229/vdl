#pragma once
#include "../ISwapChain.hpp"

#include <vdl/Texture/DirectX12/CTexture.hpp>

#include <vdl/pch/DirectX12/pch.hpp>

#include <vdl/Texture.hpp>

#include <vector>

class IWindow;
class CDeviceContext;

class CSwapChain : public ISwapChain
{
  IWindow* pWindow_;
  CDeviceContext* pDeviceContext_;
private:
  Microsoft::WRL::ComPtr<IDXGISwapChain4> pSwapChain_;
  std::vector<CRenderTexture> D3D12RenderTextures_;
  vdl::uint CurrentBufferIndex_ = 0;
private:
  vdl::RenderTextures RenderTextures_;
  vdl::DepthStencilTexture DepthStencilTexture_;
public:
    [[nodiscard]] IDXGISwapChain4* GetSwapChain() { return pSwapChain_.Get(); }
    [[nodiscard]] CRenderTexture* GetD3D12RenderTexture() { return &D3D12RenderTextures_[CurrentBufferIndex_]; }
public:
  CSwapChain() = default;

  PlatformType GetPlatform()const final { return PlatformType::eDirectX12; }

  void Initialize()override;

  void ScreenClear()override;

  void Present()override;

  void ChangeWindowMode()override;

  void ScreenShot()override;

  const vdl::RenderTexture& GetRenderTexture()const override { return RenderTextures_[0]; }

  const vdl::DepthStencilTexture& GetDepthStencilTexture()const override { return DepthStencilTexture_; }
};
