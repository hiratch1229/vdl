#include <vdl/Camera.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Input/Keyboard/IKeyboard.hpp>
#include <vdl/Input/Mouse/IMouse.hpp>

#include <vdl/Keyboard.hpp>
#include <vdl/Mouse.hpp>
#include <vdl/Quaternion.hpp>
#include <vdl/Matrix.hpp>

namespace vdl
{
  Matrix Camera::View()const
  {
    const float3 Z = ViewVector().Normalize();
    const float3 X = Up.Cross(Z).Normalize();
    const float3 Y = Z.Cross(X);

    const float3 NegPosition = -Position;

    return Matrix({ X, X.Dot(NegPosition) }, { Y, Y.Dot(NegPosition) }, { Z, Z.Dot(NegPosition) }, { 0.0f, 0.0f, 0.0f, 1.0f }).Transpose();
  }

  Matrix Camera::Projection(const float2& _Size)const
  {
    if (isPerspective)
    {
      const float AspectRatio = _Size.x / _Size.y;
      const float Height = 1.0f / std::tan(Radian(Fov) * 0.5f);
      const float Width = Height / AspectRatio;
      const float Range = Far / (Far - Near);

      return {
        Width, 0.0f, 0.0f, 0.0f,
        0.0f, Height, 0.0f, 0.0f,
        0.0f, 0.0f, Range, 1.0f,
        0.0f, 0.0f, -Range * Near, 0.0f
      };
    }
    else
    {
      const float Range = 1.0f / (Far - Near);

      return {
        2.0f / _Size.x, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / _Size.y, 0.0f, 0.0f,
        0.0f, 0.0f, Range, 0.0f,
        0.0f, 0.0f, -Range * Near, 1.0f
      };
    }
  }

  void FreeCamera(Camera* _pCamera)
  {
    IMouse* pMouse = Engine::Get<IMouse>();

    if (pMouse->Press(Input::Mouse::Buttons::eMiddle))
    {
      if (const int2 MouseDelta = pMouse->GetDelta(); MouseDelta != 0)
      {
        constexpr float3 kMultiple = { 0.01f, 0.01f, 0.6f };

        const float3 ViewVector = _pCamera->ViewVector();

        const float3 AxisZ = ViewVector.Normalize();
        const float3 AxisX = _pCamera->Up.Normalize().Cross(AxisZ);
        const float3 AxisY = AxisZ.Cross(AxisX);

        const float3 Move = (AxisX * -MouseDelta.x * kMultiple.x + AxisY * MouseDelta.y * kMultiple.y) * (ViewVector.Length() * kMultiple.z);

        _pCamera->Position += Move;
        _pCamera->Target += Move;
      }
    }

    //  TODO:^ã,^‰º‚ÌC³
    if (pMouse->Press(Input::Mouse::Buttons::eRight))
    {
      if (const int2 MouseDelta = pMouse->GetDelta(); MouseDelta != 0)
      {
        const float3 ViewVector = _pCamera->ViewVector();

        const float3 AxisZ = ViewVector.Normalize();
        const float3 AxisX = _pCamera->Up.Normalize().Cross(AxisZ);
        const float3 AxisY = AxisZ.Cross(AxisX);

        const Matrix Rotation = Matrix::Rotate(Quaternion::RotationAxis(AxisX, MouseDelta.y * 0.01f) * Quaternion::RotationAxis(AxisY, MouseDelta.x * 0.01f));

        _pCamera->Target = _pCamera->Position + AxisZ * Rotation * ViewVector.Length();
      }
    }

    if (int Wheel = pMouse->GetWheel().y; Wheel != 0)
    {
      constexpr float kMultiple = 1.5f;

      const float3 Move = _pCamera->ViewVector().Normalize() * Wheel * kMultiple;

      _pCamera->Position += Move;
      _pCamera->Target += Move;
    }
  }
}
