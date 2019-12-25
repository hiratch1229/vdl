#pragma once
#include "Fwd.hpp"

namespace vdl::Collision3D
{
  bool isHit(const Geometry3D::Sphere& _Sphere0, const Geometry3D::Sphere& _Sphere1);

  bool isHit(const Geometry3D::Sphere& _Sphere, const Geometry3D::Capsule& _Capsule);
}
