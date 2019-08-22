#pragma once
#include "Fwd.hpp"
#include "Type3.hpp"
#include "Angle.hpp"

namespace vdl
{
  struct Camera
  {
    //  ˆÊ’u
    float3 Pos = { 0.0f, 0.0f, -10.0f };
    //  ’‹“_
    float3 Target = { 0.0f, 0.0f, 0.0f };
    //  ã•ûŒü
    float3 Up = { 0.0f, 1.0f, 0.0f };
    //  Å‹ß–Ê
    float Near = 0.1f;
    //  Å‰“–Ê
    float Far = 1000.0f;
    //  ‹–ìŠp
    Degree Fov = 30.0f;
    //  true‚Å“§‹“Š‰e
    bool isPerspective = true;
  };
}
