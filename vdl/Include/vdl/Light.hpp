#pragma once
#include "Type3.hpp"
#include "Angle.hpp"
#include "Color.hpp"

namespace vdl
{
  struct DirectinalLight
  {
    float3 Direction;
    float Itensity;
    ColorF Color;
  };

  struct PointLight
  {
    float3 Position;
    float Itensity;
    ColorF Color;
    float Range;
    float3 Padding;
  };
  template<uint Size>
  struct PointLights
  {
    static constexpr uint kSize = Size;
  public:
    PointLight Lights[kSize];
  };

  struct SpotLight
  {
    float3 Position;
    float Itensity;
    float3 Direction;
    Radian Angle;
    ColorF Color;
    float Range;
    float3 Padding;
  };
  template<uint Size>
  struct SpotLights
  {
    static constexpr uint kSize = Size;
  public:
    SpotLight Lights[kSize];
  };
}
