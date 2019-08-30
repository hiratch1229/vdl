#pragma once
#include "BlendState.hpp"
#include "DepthStencilState.hpp"
#include "RasterizerState.hpp"

namespace vdl
{
  struct GraphicsState
  {
    BlendState BlendState;
    DepthStencilState DepthSteniclState;
    RasterizerState RasterizerState;
  };
}
