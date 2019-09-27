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
  void FreeCamera(Camera* _pCamera)
  {
    if (!Engine::Get<IKeyboard>()->Press(Input::Keyboard::KeyAlt.GetCode()))
    {
      return;
    }

    IMouse* pMouse = Engine::Get<IMouse>();

    if (pMouse->Press(Input::Mouse::Buttons::eMiddle))
    {
      const float3 AxisY = _pCamera->Up.Normalize();
      const float3 AxisZ = _pCamera->ViewVector().Normalize();
      const float3 AxisX = AxisY.Cross(AxisZ);

      const float3 Move = AxisX * -pMouse->GetDelta().x * 0.01f + AxisY * pMouse->GetDelta().y * 0.01f;

      _pCamera->Position += Move;
      _pCamera->Target += Move;
    }
    if (pMouse->Press(Input::Mouse::Buttons::eRight))
    {
      const float3 AxisZ = _pCamera->ViewVector();

      const float3 AxisY = _pCamera->Up.Normalize();
      const float3 AxisX = AxisY.Cross(AxisZ.Normalize());

      const Matrix Rotation = Matrix::Rotate(Quaternion::RotationAxis(AxisX, pMouse->GetDelta().y * 0.01f) * Quaternion::RotationAxis(AxisY, -pMouse->GetDelta().x * 0.01f));

      _pCamera->Position = _pCamera->Target + AxisZ * Rotation;
    }
    if (int Wheel = pMouse->GetWheel().y; Wheel != 0)
    {
      const float3 AxisZ = _pCamera->ViewVector();
      const float Length = AxisZ.Length();

      if (Wheel < 0)
      {
        _pCamera->Position -= AxisZ.Normalize() * 5.0f;
      }
      else
      {
        _pCamera->Position += AxisZ.Normalize() * Length * 0.25f;
      }
    }
  }
}
