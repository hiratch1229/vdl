#pragma once
#include "BlendState.hpp"
#include "DepthStencilState.hpp"
#include "RasterizerState.hpp"

namespace vdl
{
  struct GraphicsStates
  {
    BlendState BlendState;
    DepthStencilState DepthStencilState;
    RasterizerState RasterizerState;
  public:
    [[nodiscard]] bool operator==(const GraphicsStates& _GraphicsStates)const { return BlendState == _GraphicsStates.BlendState && DepthStencilState == _GraphicsStates.DepthStencilState && RasterizerState == _GraphicsStates.RasterizerState; }

    [[nodiscard]] bool operator!=(const GraphicsStates& _GraphicsStates)const { return BlendState != _GraphicsStates.BlendState || DepthStencilState != _GraphicsStates.DepthStencilState || RasterizerState != _GraphicsStates.RasterizerState; }
  };
}
