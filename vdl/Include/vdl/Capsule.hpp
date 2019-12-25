#pragma once
#include "Fwd.hpp"
#include "Type3.hpp"

namespace vdl::Geometry3D
{
  struct Capsule
  {
    float3 Position;
    float3 Direction;
    float Radius;
  public:
    Capsule() = default;

    constexpr Capsule(const float3& _Position, const float3& _Direction, float _Radius)noexcept
      : Position(_Position), Direction(_Direction), Radius(_Radius) {}

    constexpr Capsule(const float3& _Direction, float _Radius)noexcept
      : Position(0.0f), Direction(_Direction), Radius(_Radius) {}
  public:
    [[nodiscard]] constexpr bool operator==(const Capsule& _Capsule)const noexcept { return Position == _Capsule.Position && Direction == _Capsule.Direction && Radius == _Capsule.Radius; }

    [[nodiscard]] constexpr bool operator!=(const Capsule& _Capsule)const noexcept { return Position != _Capsule.Position || Direction != _Capsule.Direction || Radius != _Capsule.Radius; }
  };
}
