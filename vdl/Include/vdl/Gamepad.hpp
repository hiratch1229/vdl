#pragma once
#include "Fwd.hpp"
#include "Type2.hpp"
#include "Button.hpp"

namespace vdl::Input
{
  class Gamepad
  {
  public:
    enum Buttons
    {
      eUp,
      eDown,
      eLeft,
      eRight,
    };
    enum ButtonsForXInput
    {
      eStart = 4,
      eBack,
      eLeftStick,
      eRightStick,
      eLeftShoulder,
      eRightShoulder,
      eA,
      eB,
      eX,
      eY,
    };
    enum ButtonsForPS
    {
      eSquare = 4,
      eCross,
      eCircle,
      eTriangle,
      eL1,
      eR1,
      eL2,
      eR2,
      eShare,
      eOptions,
      eL3,
      eR3,
      ePS,
      eTouchPad,
    };
  private:
    uint8_t Index_;
  public:
    Gamepad() = default;

    constexpr Gamepad(uint8_t _Index)noexcept
      : Index_(_Index) {}
  public:
    [[nodiscard]] constexpr bool operator==(const Gamepad& _Gamepad)const { return Index_ == _Gamepad.Index_; }

    [[nodiscard]] constexpr bool operator!=(const Gamepad& _Gamepad)const { return Index_ != _Gamepad.Index_; }
  public:
    [[nodiscard]] constexpr uint8_t GetIndex()const noexcept { return Index_; }

    [[nodiscard]] Button GetButton(uint8_t _Number)const noexcept { return { InputDeviceType::eGamepad, _Number, Index_ }; }

    [[nodiscard]] uint GetButtonNum()const;
  };

  //  �ڑ�����Ă��邩�m�F
  //  �ڑ�����true
  [[nodiscard]] bool isConnect(const Gamepad& _Gamepad);

  //  ���X�e�B�b�N�̓��͏�Ԃ��擾
  //  -1~1�͈̔͂ŕ\������܂�
  [[nodiscard]] float2 GetLeftStick(const Gamepad& _Gamepad, float _DeadZone = 0.0f);

  //  �E�X�e�B�b�N�̓��͏�Ԃ��擾
  //  -1~1�͈̔͂ŕ\������܂�
  [[nodiscard]] float2 GetRightStick(const Gamepad& _Gamepad, float _DeadZone = 0.0f);

  //  ���g���K�[�̓��͏�Ԃ��擾
  //  0-1�͈̔͂ŕ\������܂�
  [[nodiscard]] float GetLeftTrigger(const Gamepad& _Gamepad, float _DeadZone = 0.0f);

  //  �E�g���K�[�̓��͏�Ԃ��擾
  //  0-1�͈̔͂ŕ\������܂�
  [[nodiscard]] float GetRightTrigger(const Gamepad& _Gamepad, float _DeadZone = 0.0f);

  //  �R���g���[����U�������܂�
  //  0�͒�~�A1�͍ő呬�x
  void SetVibration(const Gamepad& _Gamepad, float _Speed);

  //  �R���g���[���̐U�����~�߂܂�
  void StopVibration(const Gamepad& _Gamepad);

  //  ����������Ă���Ȃ�true��Ԃ�
  inline bool isPressAnyButtonOnGamepad(const Gamepad& _Gamepad)
  {
    const uint ButtonNum = _Gamepad.GetButtonNum();
    for (uint ButtonCount = 0; ButtonCount < ButtonNum; ++ButtonCount)
    {
      if (isPress(_Gamepad.GetButton(ButtonCount)))
      {
        return true;
      }
    }

    return false;
  }

  //  ���������ꂽ�u�ԂȂ�true��Ԃ�
  inline bool isPressedAnyButtonOnGamepad(const Gamepad& _Gamepad)
  {
    const uint ButtonNum = _Gamepad.GetButtonNum();
    for (uint ButtonCount = 0; ButtonCount < ButtonNum; ++ButtonCount)
    {
      if (isPressed(_Gamepad.GetButton(ButtonCount)))
      {
        return true;
      }
    }

    return false;
  }

  //  ���������ꂽ�u�ԂȂ�true��Ԃ�
  inline bool isReleasedAnyButtonOnGamepad(const Gamepad& _Gamepad)
  {
    const uint ButtonNum = _Gamepad.GetButtonNum();
    for (uint ButtonCount = 0; ButtonCount < ButtonNum; ++ButtonCount)
    {
      if (isReleased(_Gamepad.GetButton(ButtonCount)))
      {
        return true;
      }
    }

    return false;
  }

  //  ����������Ă��Ȃ��Ȃ�true��Ԃ�
  inline bool isReleaseAnyButtonOnGamepad(const Gamepad& _Gamepad)
  {
    const uint ButtonNum = _Gamepad.GetButtonNum();
    for (uint ButtonCount = 0; ButtonCount < ButtonNum; ++ButtonCount)
    {
      if (isRelease(_Gamepad.GetButton(ButtonCount)))
      {
        return true;
      }
    }

    return false;
  }
}
