#pragma once
#include "Fwd.hpp"
#include "Type2.hpp"
#include "Texture.hpp"

namespace vdl::Window
{
  [[nodiscard]] uint2 GetWindowSize();

  [[nodiscard]] RenderTexture GetRenderTexture();

  [[nodiscard]] DepthStencilTexture GetDepthStencilTexture();
}
