#pragma once
#include "Fwd.hpp"
#include "Texture.hpp"
#include "GraphicsState.hpp"

namespace vdl::RendererTexture
{
  void SetScissor(const Scissor& _Scissor);

  void SetViewport(const Viewport& _Viewport);

  void SetBlendState(const BlendState& _BlendState);

  void SetDepthStencilState(const DepthStencilState& _DepthStencilState);

  void SetRasterizerState(const RasterizerState& _RasterizerState);

  inline void SetGraphicsState(const GraphicsState& _GraphicsState)
  {
    SetBlendState(_GraphicsState.BlendState);
    SetDepthStencilState(_GraphicsState.DepthSteniclState);
    SetRasterizerState(_GraphicsState.RasterizerState);
  }

  void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _DestSize, const float2& _SrcLeftPos, const float2& _SrcSize, const Degree& _Angle, const ColorF& _Color = Palette::White);

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _DestSize, const float2& _SrcLeftPos, const float2& _SrcSize, const ColorF& _Color = Palette::White)
  {
    Draw(_Texture, _DestLeftTop, _SrcSize, _SrcLeftPos, _SrcSize, 0.0f, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _SrcLeftPos, const float2& _SrcSize, const Degree& _Angle, const ColorF& _Color = Palette::White)
  {
    Draw(_Texture, _DestLeftTop, _SrcSize, _SrcLeftPos, _SrcSize, _Angle, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _SrcLeftPos, const float2& _SrcSize, const ColorF& _Color = Palette::White)
  {
    Draw(_Texture, _DestLeftTop, _SrcSize, _SrcLeftPos, _SrcSize, 0.0f, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _DestSize, const Degree& _Angle, const ColorF& _Color = Palette::White)
  {
    Draw(_Texture, _DestLeftTop, _DestSize, float2(0.0f), _Texture.GetSize(), _Angle, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _DestSize, const ColorF& _Color = Palette::White)
  {
    Draw(_Texture, _DestLeftTop, _DestSize, float2(0.0f), _Texture.GetSize(), 0.0f, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const Degree& _Angle, const ColorF& _Color = Palette::White)
  {
    const uint2 Size = _Texture.GetSize();
    Draw(_Texture, _DestLeftTop, Size, float2(0.0f), Size, _Angle, _Color);
  }

  inline void Draw(const Texture& _Texture, const float2& _DestLeftTop, const ColorF& _Color = Palette::White)
  {
    const uint2 Size = _Texture.GetSize();
    Draw(_Texture, _DestLeftTop, Size, float2(0.0f), Size, 0.0f, _Color);
  }
}
