#pragma once
#include "Fwd.hpp"
#include "Type3.hpp"
#include "Angle.hpp"
#include "Color.hpp"

namespace vdl
{
  struct DirectinalLight
  {
    float3 Direction;
    float Itensity;
    Color4F Color;
  };

  struct PointLight
  {
    float3 Position;
    float Itensity;
    Color4F Color;
    float3 Padding;
    float Range;
  };
  template<uint Size>
  struct PointLights
  {
    static constexpr uint kSize = Size;
  public:
    PointLight Lights[kSize];
  public:
    PointLight& operator[](vdl::uint i) { return Lights[i]; }
  };

  struct SpotLight
  {
    float3 Position;
    float Itensity;
    float3 Direction;
    Radian Angle;
    Color4F Color;
    float Range;
    float3 Padding;
  };
  template<uint Size>
  struct SpotLights
  {
    static constexpr uint kSize = Size;
  public:
    SpotLight Lights[kSize];
  public:
    SpotLight& operator[](vdl::uint i) { return Lights[i]; }
  };
}
