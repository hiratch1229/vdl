#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Texture.hpp>

#include <vdl/pch/DirectX11/pch.hpp>

class SwapChain
{
  IDXGISwapChain* pSwapChain_;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView_;
  vdl::RenderTextures RenderTextures_;
  vdl::DepthStencilTexture DepthStencilTexture_;
public:
  [[nodiscard]] const vdl::RenderTexture& GetRenderTexture()const { return RenderTextures_[0]; }

  [[nodiscard]] const vdl::RenderTextures& GetRenderTextures()const { return RenderTextures_; }

  [[nodiscard]] const vdl::DepthStencilTexture& GetDepthStencilTexture()const { return DepthStencilTexture_; }

  [[nodiscard]] IDXGISwapChain* operator->()const { return pSwapChain_; }
public:
  SwapChain() = default;

  void Initialize(ID3D11Device* _pDevice, IDXGISwapChain* _pSwapChain);
};
