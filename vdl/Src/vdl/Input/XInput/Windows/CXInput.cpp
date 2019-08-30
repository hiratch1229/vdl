#include "CXInput.hpp"

#include "vdl/Math.hpp"

#include <Windows.h>
#include <Xinput.h>

void CXInput::Initialize()
{

}

void CXInput::Update()
{
  static constexpr int kXInputButtonsCode[] = { XINPUT_GAMEPAD_DPAD_UP, XINPUT_GAMEPAD_DPAD_DOWN, XINPUT_GAMEPAD_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_RIGHT,
    XINPUT_GAMEPAD_START, XINPUT_GAMEPAD_BACK, XINPUT_GAMEPAD_LEFT_THUMB, XINPUT_GAMEPAD_RIGHT_THUMB, XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER,
    XINPUT_GAMEPAD_A, XINPUT_GAMEPAD_B, XINPUT_GAMEPAD_X, XINPUT_GAMEPAD_Y };

  XINPUT_STATE XInputState;

  for (int i = 0; i < Constants::kMaxController; ++i)
  {
    Status& Status = Status_[i];

    //  �ڑ�����Ă��Ȃ���
    if (Status.isConnect = ::XInputGetState(i, &XInputState) != ERROR_DEVICE_NOT_CONNECTED;
      !Status.isConnect)
    {
      continue;
    }

    auto& Gamepad = XInputState.Gamepad;
    for (int j = 0; j < Constants::kXInputButtonNum; ++j)
    {
      Status.InputStatus[j].Update((Gamepad.wButtons & kXInputButtonsCode[j]) != 0);
    }

    //  ��Ԃ�ۑ�
    {
      //  �A�i���O�X�e�B�b�N
      {
        Status.LeftStick = ((vdl::float2(Gamepad.sThumbLX, Gamepad.sThumbLY) - SHRT_MIN) * 2.0f) / kAnalogStickMaxValue - 1.0f;
        Status.RightStick = ((vdl::float2(Gamepad.sThumbRX, Gamepad.sThumbRY) - SHRT_MIN) * 2.0f) / kAnalogStickMaxValue - 1.0f;
      }

      //  �g���K�[
      {
        Status.LeftTrigger = Gamepad.bLeftTrigger / kTriggerMaxValue;
        Status.RightTrigger = Gamepad.bRightTrigger / kTriggerMaxValue;
      }
    }
  }
}

vdl::float2 CXInput::GetLeftStick(int _Index, float _DeadZone)const
{
  if (!isWithinRange(_Index))
  {
    return vdl::float2(0.0f, 0.0f);
  }

  vdl::float2 Value = Status_[_Index].LeftStick;

  if (vdl::math::GetAbsoluteValue(Value.x) < _DeadZone) Value.x = 0.0f;
  if (vdl::math::GetAbsoluteValue(Value.y) < _DeadZone) Value.y = 0.0f;

  return Value;
}

vdl::float2 CXInput::GetRightStick(int _Index, float _DeadZone)const
{
  if (!isWithinRange(_Index))
  {
    return vdl::float2(0.0f, 0.0f);
  }

  vdl::float2 Value = Status_[_Index].RightStick;

  if (vdl::math::GetAbsoluteValue(Value.x) < _DeadZone) Value.x = 0.0f;
  if (vdl::math::GetAbsoluteValue(Value.y) < _DeadZone) Value.y = 0.0f;

  return Value;
}

float CXInput::GetLeftTrigger(int _Index, float _DeadZone)const
{
  if (!isWithinRange(_Index))
  {
    return 0.0f;
  }

  float Value = Status_[_Index].LeftTrigger;

  if (Value < _DeadZone) Value = 0.0f;

  return Value;
}

float CXInput::GetRightTrigger(int _Index, float _DeadZone)const
{
  if (!isWithinRange(_Index))
  {
    return 0.0f;
  }

  float Value = Status_[_Index].RightTrigger;

  if (Value < _DeadZone) Value = 0.0f;

  return Value;
}

void CXInput::SetVibration(int _Index, float _Speed)const
{
  static constexpr WORD kMaxSpeed = 65535;

  const WORD MotorSpeed = static_cast<WORD>(kMaxSpeed * _Speed);

  XINPUT_VIBRATION Vibration{ MotorSpeed, MotorSpeed };

  ::XInputSetState(_Index, &Vibration);
}

void CXInput::StopVibration(int _Index)const
{
  XINPUT_VIBRATION Vibration{ 0, 0 };

  ::XInputSetState(_Index, &Vibration);
}
