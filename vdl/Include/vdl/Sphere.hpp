#pragma once
#include "Fwd.hpp"
#include "Type3.hpp"

namespace vdl::Geometry3D
{
  struct Sphere
  {
    float3 Position;
    float Radius;
  public:
    Sphere() = default;

    constexpr Sphere(const float3& _Position, float _Radius)noexcept
      : Position(_Position), Radius(_Radius) {}

    constexpr Sphere(float _Radius)noexcept
      : Position(0.0f), Radius(_Radius) {}
  public:
    [[nodiscard]] constexpr bool operator==(const Sphere& _Sphere)const noexcept { return Position == _Sphere.Position && Radius == _Sphere.Radius; }

    [[nodiscard]] constexpr bool operator!=(const Sphere& _Sphere)const noexcept { return Position != _Sphere.Position || Radius != _Sphere.Radius; }
  };
}
