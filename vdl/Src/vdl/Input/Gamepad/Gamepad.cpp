#include <vdl/Gamepad.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Input/Gamepad/IGamepad.hpp>

namespace vdl::Input
{
  uint Gamepad::GetButtonNum()const
  {
    return Engine::Get<IGamepad>()->GetButtonNum(Index_);
  }

  //--------------------------------------------------

  bool isConnect(const Gamepad& _Gamepad)
  {
    return Engine::Get<IGamepad>()->isConnect(_Gamepad.GetIndex());
  }

  float2 GetLeftStick(const Gamepad& _Gamepad, float _DeadZone)
  {
    return Engine::Get<IGamepad>()->GetLeftStick(_Gamepad.GetIndex(), _DeadZone);
  }

  float2 GetRightStick(const Gamepad& _Gamepad, float _DeadZone)
  {
    return Engine::Get<IGamepad>()->GetRightStick(_Gamepad.GetIndex(), _DeadZone);
  }

  float GetLeftTrigger(const Gamepad& _Gamepad, float _DeadZone)
  {
    return Engine::Get<IGamepad>()->GetLeftTrigger(_Gamepad.GetIndex(), _DeadZone);
  }

  float GetRightTrigger(const Gamepad& _Gamepad, float _DeadZone)
  {
    return Engine::Get<IGamepad>()->GetRightTrigger(_Gamepad.GetIndex(), _DeadZone);
  }

  void SetVibration(const Gamepad& _Gamepad, float _Speed)
  {
    Engine::Get<IGamepad>()->SetVibration(_Gamepad.GetIndex(), _Speed);
  }

  void StopVibration(const Gamepad& _Gamepad)
  {
    Engine::Get<IGamepad>()->StopVibration(_Gamepad.GetIndex());
  }
}
