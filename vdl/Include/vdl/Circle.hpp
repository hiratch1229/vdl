#pragma once
#include "Fwd.hpp"
#include "Type2.hpp"

namespace vdl::Geometry2D
{
  struct Circle
  {
    float2 Center;
    float Radius;
  public:
    Circle() = default;

    constexpr Circle(const float2& _Center, float _Radius)noexcept
      : Center(_Center), Radius(_Radius) {}

    constexpr Circle(float _Radius)noexcept
      : Center(0.0f), Radius(_Radius) {}
  public:
    [[nodiscard]] constexpr bool operator==(const Circle& _Circle)const noexcept { return Center == _Circle.Center && Radius == _Circle.Radius; }

    [[nodiscard]] constexpr bool operator!=(const Circle& _Circle)const noexcept { return Center != _Circle.Center || Radius != _Circle.Radius; }
  };
}
