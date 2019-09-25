#pragma once
#include "Fwd.hpp"
#include "Type3.hpp"
#include "Angle.hpp"

namespace vdl
{
  struct Camera
  {
    float3 Position;
    float3 Target;
    float3 Up;
    float Near;
    float Far;
    Degree Fov;
    bool isPerspective;
  public:
    Camera() = default;

    Camera(const float3& _Position, const float3& _Target = 0.0f, const float3 & _Up = float3::Up(),
      float _Near = 0.1f, float _Far = 1000.0f, const Degree & _Fov = 30.0f, bool _isPerspective = true)
      : Position(_Position), Target(_Target), Up(_Up), Near(_Near), Far(_Far), Fov(_Fov), isPerspective(_isPerspective) {}
  public:
    [[nodiscard]] bool operator==(const Camera& _Camera)const { return Position == _Camera.Position && Target == _Camera.Target && Up == _Camera.Up && Near == _Camera.Near && Far == _Camera.Far && Fov == _Camera.Fov && isPerspective == _Camera.isPerspective; }

    [[nodiscard]] bool operator!=(const Camera& _Camera)const { return Position != _Camera.Position || Target != _Camera.Target || Up != _Camera.Up || Near != _Camera.Near || Far != _Camera.Far || Fov != _Camera.Fov || isPerspective != _Camera.isPerspective; }
  };
}
