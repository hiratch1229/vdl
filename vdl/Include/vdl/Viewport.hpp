#pragma once
#include "Type2.hpp"

namespace vdl
{
  struct Viewport
  {
    float2 LeftTop;
    float2 Size;
    float MinDepth;
    float MaxDepth;
  public:
    Viewport() = default;

    Viewport(float _LeftTopX, float _LeftTopY, float _Width, float _Height, float _MinDepth = 0.0f, float _MaxDepth = 1.0f)
      : LeftTop(_LeftTopX, _LeftTopY), Size(_Width, _Height), MinDepth(_MinDepth), MaxDepth(_MaxDepth) {}

    Viewport(const float2& _LeftTop, const float2& _Size, float _MinDepth = 0.0f, float _MaxDepth = 1.0f)
      : LeftTop(_LeftTop), Size(_Size), MinDepth(_MinDepth), MaxDepth(_MaxDepth) {}
  public:
    [[nodiscard]] constexpr bool operator==(const Viewport& _Viewport)const { return LeftTop == _Viewport.LeftTop && Size == _Viewport.Size && MinDepth == _Viewport.MinDepth && MaxDepth == _Viewport.MaxDepth; }
    
    [[nodiscard]] constexpr bool operator!=(const Viewport& _Viewport)const { return LeftTop != _Viewport.LeftTop || Size != _Viewport.Size || MinDepth != _Viewport.MinDepth || MaxDepth != _Viewport.MaxDepth; }
  };
}
