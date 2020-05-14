#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Texture.hpp>

#include <vdl/pch/DirectX12/pch.hpp>
#include <vdl/Texture/DirectX12/CTexture.hpp>

class CDevice;

class SwapChain
{
  IDXGISwapChain4* pSwapChain_;
  std::vector<CRenderTexture> D3D12RenderTextures_;
private:
  vdl::RenderTextures RenderTextures_;
  vdl::DepthStencilTexture DepthStencilTexture_;
public:
  [[nodiscard]] CRenderTexture* GetD3D12RenderTexture(vdl::uint _Index) { return &D3D12RenderTextures_[_Index]; }

  [[nodiscard]] const vdl::RenderTexture& GetRenderTexture()const { return RenderTextures_[0]; }

  [[nodiscard]] const vdl::RenderTextures& GetRenderTextures()const { return RenderTextures_; }

  [[nodiscard]] const vdl::DepthStencilTexture& GetDepthStencilTexture()const { return DepthStencilTexture_; }

  [[nodiscard]] IDXGISwapChain4* operator->()const { return pSwapChain_; }
public:
  SwapChain() = default;

  void Initialize(CDevice* _pDevice);
};
