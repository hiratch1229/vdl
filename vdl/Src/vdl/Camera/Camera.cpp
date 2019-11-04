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
    return DirectX::XMMatrixLookAtLH({ Position.x, Position.y, Position.z, 1.0f },
      { Target.x, Target.y, Target.z, 1.0f }, { Up.x, Up.y, Up.z, 0.0f });
  }

  Matrix Camera::Projection(const float2& _Size)const
  {
    if (isPerspective)
    {
      return DirectX::XMMatrixPerspectiveFovLH(vdl::Math::ToRadian(Fov), _Size.x / _Size.y, Near, Far);
    }
    else
    {
      return DirectX::XMMatrixOrthographicLH(_Size.x, _Size.y, Near, Far);
    }
  }

  void FreeCamera(Camera* _pCamera)
  {
    IMouse* pMouse = Engine::Get<IMouse>();

    if (pMouse->Press(Input::Mouse::Buttons::eMiddle))
    {
      if (const int2 MouseDelta = pMouse->GetDelta(); MouseDelta != 0)
      {
        constexpr float3 kMultiple = { 0.01f, 0.01f, 0.1f };

        const float3 ViewVector = _pCamera->ViewVector();

        const float3 AxisZ = ViewVector.Normalize();
        const float3 AxisX = _pCamera->Up.Normalize().Cross(AxisZ);
        const float3 AxisY = AxisZ.Cross(AxisX);

        const float3 Move = (AxisX * -MouseDelta.x * kMultiple.x + AxisY * MouseDelta.y * kMultiple.y) * (ViewVector.Length() * kMultiple.z);

        _pCamera->Position += Move;
        _pCamera->Target += Move;
      }
    }
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
