#pragma once
#include "Fwd.hpp"

namespace vdl::Collision3D
{
  [[nodiscard]] bool isHit(const Geometry3D::Sphere& _Sphere0, const Geometry3D::Sphere& _Sphere1)noexcept;

  [[nodiscard]] bool isHit(const Geometry3D::Sphere& _Sphere, const Geometry3D::Capsule& _Capsule)noexcept;
}
