#pragma once
#include "Fwd.hpp"

#include "Color.hpp"
#include "Viewport.hpp"

namespace vdl::Renderer
{
  void SetRenderTextures(const RenderTextures& _RenderTextures, const DepthStencilTexture& _DepthStencilTexture);

  inline void SetRenderTexture(const RenderTexture& _RenderTexture, const DepthStencilTexture& _DepthStencilTexture)
  {
    SetRenderTextures({ _RenderTexture }, _DepthStencilTexture);
  }

  void Clear(const RenderTexture& _RenderTexure, const ColorF& _ClearColor = Palette::Black);

  void Clear(const DepthStencilTexture& _DepthStencilTexture, float _ClearDepth = 1.0f, uint _ClearStencil = 0);
}
