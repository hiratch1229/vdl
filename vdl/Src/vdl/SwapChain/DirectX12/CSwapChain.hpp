#pragma once
#include "../ISwapChain.hpp"

class CSwapChain : public ISwapChain
{
private:
  vdl::RenderTextures RenderTextures_;
  vdl::DepthStencilTexture DepthStencilTexture_;
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
