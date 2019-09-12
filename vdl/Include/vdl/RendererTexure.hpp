#pragma once
#include "Fwd.hpp"
#include "Texture.hpp"
#include "GraphicsState.hpp"

namespace vdl::RendererTexture
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

  void Draw(const Texture& _Texture, const float2& _DstLeftTop, const float2& _DstSize, const float2& _SrcLeftPos, const float2& _SrcSize, const Degree& _Angle, const ColorF& _Color = Palette::White);

  inline void Draw(const Texture& _Texture, const float2& _DstLeftTop, const float2& _DstSize, const float2& _SrcLeftPos, const float2& _SrcSize, const ColorF& _Color = Palette::White)
  {
    Draw(_Texture, _DstLeftTop, _SrcSize, _SrcLeftPos, _SrcSize, 0.0f, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DstLeftTop, const float2& _SrcLeftPos, const float2& _SrcSize, const Degree& _Angle, const ColorF& _Color = Palette::White)
  {
    Draw(_Texture, _DstLeftTop, _SrcSize, _SrcLeftPos, _SrcSize, _Angle, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DstLeftTop, const float2& _SrcLeftPos, const float2& _SrcSize, const ColorF& _Color = Palette::White)
  {
    Draw(_Texture, _DstLeftTop, _SrcSize, _SrcLeftPos, _SrcSize, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DstLeftTop, const float2& _DstSize, const Degree& _Angle, const ColorF& _Color = Palette::White)
  {
    Draw(_Texture, _DstLeftTop, _DstSize, int2(0, 0), _Texture.GetSize(), _Angle, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DstLeftTop, const float2& _DstSize, const ColorF& _Color = Palette::White)
  {
    Draw(_Texture, _DstLeftTop, _DstSize, int2(0, 0), _Texture.GetSize(), _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DstLeftTop, const Degree& _Angle, const ColorF& _Color = Palette::White)
  {
    const uint2 Size = _Texture.GetSize();
    Draw(_Texture, _DstLeftTop, Size, int2(0, 0), Size, _Angle, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DstLeftTop, const ColorF& _Color = Palette::White)
  {
    const uint2 Size = _Texture.GetSize();
    Draw(_Texture, _DstLeftTop, Size, int2(0, 0), Size, _Color);
  }
}
