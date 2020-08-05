#include <vdl/Collision2D.hpp>

#include <vdl/Circle.hpp>

namespace vdl::Collision2D
{
  bool isHit(const Geometry2D::Circle& _Circle0, const Geometry2D::Circle& _Circle1)noexcept
  {
    return (_Circle1.Center - _Circle0.Center).LengthSq() <= ((_Circle0.Radius + _Circle1.Radius) * (_Circle0.Radius + _Circle1.Radius));
  }
}
