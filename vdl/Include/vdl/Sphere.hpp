#pragma once
#include "Fwd.hpp"
#include "Type3.hpp"

namespace vdl::Geometry3D
{
  struct Sphere
  {
    float3 Center;
    float Radius;
  public:
    Sphere() = default;

    constexpr Sphere(const float3& _Center, float _Radius)noexcept
      : Center(_Center), Radius(_Radius) {}

    constexpr Sphere(float _Radius)noexcept
      : Center(0.0f), Radius(_Radius) {}
  public:
    [[nodiscard]] constexpr bool operator==(const Sphere& _Sphere)const noexcept { return Center == _Sphere.Center && Radius == _Sphere.Radius; }

    [[nodiscard]] constexpr bool operator!=(const Sphere& _Sphere)const noexcept { return Center != _Sphere.Center || Radius != _Sphere.Radius; }
  };
}
