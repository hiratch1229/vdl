#pragma once
#include "Fwd.hpp"
#include "Model.hpp"

namespace vdl::RendererStaticMesh
{
  void SetBlendState(const BlendState& _BlendState);

  void SetDepthStencilState(const DepthStencilState& _DepthStencilState);

  void SetRasterizerState(const RasterizerState& _RasterizerState);

  inline void SetGraphicsState(const GraphicsState& _GraphicsState)
  {
    SetBlendState(_GraphicsState.BlendState);
    SetDepthStencilState(_GraphicsState.DepthSteniclState);
    SetRasterizerState(_GraphicsState.RasterizerState);
  }

  void Draw(const StaticMesh& _StaticMesh, const Matrix& _World, const ColorF& _Color = Palette::White);

  inline void Draw(const Model& _Model, const Matrix& _World, const ColorF& _Color = Palette::White)
  {
    for (auto& Mesh : _Model.GetMeshes())
    {
      Draw(Mesh, _World, _Color);
    }
  }
}
