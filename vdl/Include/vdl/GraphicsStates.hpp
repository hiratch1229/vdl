#pragma once
#include "BlendState.hpp"
#include "DepthStencilState.hpp"
#include "RasterizerState.hpp"

namespace vdl
{
  struct GraphicsStates
  {
    BlendState BlendState;
    DepthStencilState DepthSteniclState;
    RasterizerState RasterizerState;
  public:
    [[nodiscard]] bool operator==(const GraphicsStates& _GraphicsStates)const { return BlendState == _GraphicsStates.BlendState && DepthSteniclState == _GraphicsStates.DepthSteniclState && RasterizerState == _GraphicsStates.RasterizerState; }

    [[nodiscard]] bool operator!=(const GraphicsStates& _GraphicsStates)const { return BlendState != _GraphicsStates.BlendState || DepthSteniclState != _GraphicsStates.DepthSteniclState || RasterizerState != _GraphicsStates.RasterizerState; }
  };
}
