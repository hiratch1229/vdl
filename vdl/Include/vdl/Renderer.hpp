#pragma once
#include "Fwd.hpp"

namespace vdl::Renderer
{
  void SetScissor(const Scissor& _Scissor);

  void SetViewport(const Viewport& _Viewport);

  void SetRenderTextures(const RenderTexture& _RenderTexture, const DepthStencilTexture& _DepthStencilTexture);

  void Clear(const RenderTexture& _RenderTexure, const ColorF& _ClearColor = Palette::Black);

  void Clear(const DepthStencilTexture& _DepthStencilTexture, float _ClearDepth = 1.0f, uint _ClearStencil = 0);
}
