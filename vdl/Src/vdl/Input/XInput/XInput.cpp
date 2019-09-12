#include <vdl/XInput.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Input/XInput/IXInput.hpp>

namespace vdl::Input
{
  bool isConnect(const XInput& _XInput)
  {
    return Engine::Get<IXInput>()->isConnect(_XInput.GetIndex());
  }

  float2 GetLeftStick(const XInput& _XInput, float _DeadZone)
  {
    return Engine::Get<IXInput>()->GetLeftStick(_XInput.GetIndex(), _DeadZone);
  }

  float2 GetRightStick(const XInput& _XInput, float _DeadZone)
  {
    return Engine::Get<IXInput>()->GetRightStick(_XInput.GetIndex(), _DeadZone);
  }

  float GetLeftTrigger(const XInput& _XInput, float _DeadZone)
  {
    return Engine::Get<IXInput>()->GetLeftTrigger(_XInput.GetIndex(), _DeadZone);
  }

  float GetRightTrigger(const XInput& _XInput, float _DeadZone)
  {
    return Engine::Get<IXInput>()->GetRightTrigger(_XInput.GetIndex(), _DeadZone);
  }

  void SetVibration(const XInput& _XInput, float _Speed)
  {
    Engine::Get<IXInput>()->SetVibration(_XInput.GetIndex(), _Speed);
  }

  void StopVibration(const XInput& _XInput)
  {
    Engine::Get<IXInput>()->StopVibration(_XInput.GetIndex());
  }
}
