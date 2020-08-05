#pragma once
#include "Fwd.hpp"

namespace vdl::Collision2D
{
  [[nodiscard]] bool isHit(const Geometry2D::Circle& _Circle0, const Geometry2D::Circle& _Circle1)noexcept;
}
