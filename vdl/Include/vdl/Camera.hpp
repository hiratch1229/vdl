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

    constexpr Camera(const float3& _Position, const float3& _Target = 0.0f, const float3 & _Up = float3::Up(),
      float _Near = 0.1f, float _Far = 1000.0f, const Degree & _Fov = 30.0f, bool _isPerspective = true)
      : Position(_Position), Target(_Target), Up(_Up), Near(_Near), Far(_Far), Fov(_Fov), isPerspective(_isPerspective) {}
  public:
    [[nodiscard]] float3 ViewVector()const { return Target - Position; }

    [[nodiscard]] Matrix View()const;

    [[nodiscard]] Matrix Projection(const vdl::float2& _Size)const;
  };

  void FreeCamera(Camera* _pCamera);
}
