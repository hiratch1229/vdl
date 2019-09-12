#pragma once
#include "Fwd.hpp"
#include "Model.hpp"
#include "MotionData.hpp"

namespace vdl::RendererSkinnedMesh
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

  void Draw(const SkinnedMesh& _SkinnedMesh, const Matrix& _World, const MotionData& _MotionData, const ColorF& _Color = Palette::White);

  void Draw(const SkinnedMesh& _SkinnedMesh, const Matrix& _World, const MotionBlendDatas& _MotionBlendDatas, const ColorF& _Color = Palette::White);

  inline void Draw(const Model& _Model, const Matrix& _World, const MotionData& _MotionData, const ColorF& _Color = Palette::White)
  {
    for (auto& Mesh : _Model.GetMeshes())
    {
      Draw(Mesh, _World, _MotionData, _Color);
    }
  }

  inline void Draw(const Model& _Model, const Matrix& _World, const std::vector<MotionBlendData>& _MotionBlendDatas, const ColorF& _Color = Palette::White)
  {
    for (auto& Mesh : _Model.GetMeshes())
    {
      Draw(Mesh, _World, _MotionBlendDatas, _Color);
    }
  }
}
