#pragma once
#include "Button.hpp"
#include "Type2.hpp"

namespace vdl::Input
{
  class XInput
  {
  public:
    enum Buttons
    {
      eUp,
      eDown,
      eLeft,
      eRight,
      eStart,
      eBack,
      eLeftStick,
      eRightStick,
      eLB,
      eRB,
      eA,
      eB,
      eX,
      eY,
    };
  private:
    uint8_t Index_;
  public:
    XInput() = default;

    constexpr XInput(uint8_t _Index)noexcept
      : Index_(_Index) {}
  public:
    [[nodiscard]] constexpr bool operator==(const XInput& _XInput)const noexcept { return Index_ == _XInput.Index_; }

    [[nodiscard]] constexpr bool operator!=(const XInput& _XInput)const noexcept { return Index_ != _XInput.Index_; }
  public:
    [[nodiscard]] constexpr uint8_t GetIndex()const noexcept { return Index_; }
  public:
    [[nodiscard]] constexpr Button ButtonUp()const noexcept { return { InputDevice::eXInput, Buttons::eUp, Index_ }; }

    [[nodiscard]] constexpr Button ButtonDown()const noexcept { return { InputDevice::eXInput, Buttons::eDown, Index_ }; }

    [[nodiscard]] constexpr Button ButtonLeft()const noexcept { return { InputDevice::eXInput, Buttons::eLeft, Index_ }; }

    [[nodiscard]] constexpr Button ButtonRight()const noexcept { return { InputDevice::eXInput, Buttons::eRight, Index_ }; }

    [[nodiscard]] constexpr Button ButtonStart()const noexcept { return { InputDevice::eXInput, Buttons::eStart, Index_ }; }

    [[nodiscard]] constexpr Button ButtonSelect()const noexcept { return { InputDevice::eXInput, Buttons::eBack, Index_ }; }

    [[nodiscard]] constexpr Button ButtonLeftStick()const noexcept { return { InputDevice::eXInput, Buttons::eLeftStick, Index_ }; }

    [[nodiscard]] constexpr Button ButtonRightStick()const noexcept { return { InputDevice::eXInput, Buttons::eRightStick, Index_ }; }

    [[nodiscard]] constexpr Button ButtonLB()const noexcept { return { InputDevice::eXInput, Buttons::eLB, Index_ }; }

    [[nodiscard]] constexpr Button ButtonRB()const noexcept { return { InputDevice::eXInput, Buttons::eRB, Index_ }; }

    [[nodiscard]] constexpr Button ButtonA()const noexcept { return { InputDevice::eXInput, Buttons::eA, Index_ }; }

    [[nodiscard]] constexpr Button ButtonB()const noexcept { return { InputDevice::eXInput, Buttons::eB, Index_ }; }

    [[nodiscard]] constexpr Button ButtonX()const noexcept { return { InputDevice::eXInput, Buttons::eX, Index_ }; }

    [[nodiscard]] constexpr Button ButtonY()const noexcept { return { InputDevice::eXInput, Buttons::eY, Index_ }; }
  };

  //  �ڑ�����Ă��邩�m�F
  //  �ڑ�����true
  [[nodiscard]] bool isConnect(const XInput& _XInput);

  //  ���X�e�B�b�N�̓��͏�Ԃ��擾
  //  -1~1�͈̔͂ŕ\������܂�
  [[nodiscard]] float2 GetLeftStick(const XInput& _XInput, float _DeadZone = 0.0f);

  //  �E�X�e�B�b�N�̓��͏�Ԃ��擾
  //  -1~1�͈̔͂ŕ\������܂�
  [[nodiscard]] float2 GetRightStick(const XInput& _XInput, float _DeadZone = 0.0f);

  //  ���g���K�[�̓��͏�Ԃ��擾
  //  0-1�͈̔͂ŕ\������܂�
  [[nodiscard]] float GetLeftTrigger(const XInput& _XInput, float _DeadZone = 0.0f);

  //  �E�g���K�[�̓��͏�Ԃ��擾
  //  0-1�͈̔͂ŕ\������܂�
  [[nodiscard]] float GetRightTrigger(const XInput& _XInput, float _DeadZone = 0.0f);
  
  //  �R���g���[����U�������܂�
  //  0�͒�~�A1�͍ő呬�x
  void SetVibration(const XInput& _XInput, float _Speed);

  //  �R���g���[���̐U�����~�߂܂�
  void StopVibration(const XInput& _XInput);

  //  ����������Ă���Ȃ�true��Ԃ�
  inline bool isPressAnyButtonOnXInput(const XInput& _XInput)
  {
    return isPressOr(_XInput.ButtonUp(), _XInput.ButtonDown(), _XInput.ButtonLeft(), _XInput.ButtonRight(),
      _XInput.ButtonStart(), _XInput.ButtonSelect(), _XInput.ButtonLeftStick(), _XInput.ButtonRightStick(),
      _XInput.ButtonLB(), _XInput.ButtonRB(), _XInput.ButtonA(), _XInput.ButtonB(), _XInput.ButtonX(), _XInput.ButtonY());
  }

  //  ���������ꂽ�u�ԂȂ�true��Ԃ�
  inline bool isPressedAnyButtonOnXInput(const XInput& _XInput)
  {
    return isPressedOr(_XInput.ButtonUp(), _XInput.ButtonDown(), _XInput.ButtonLeft(), _XInput.ButtonRight(),
      _XInput.ButtonStart(), _XInput.ButtonSelect(), _XInput.ButtonLeftStick(), _XInput.ButtonRightStick(),
      _XInput.ButtonLB(), _XInput.ButtonRB(), _XInput.ButtonA(), _XInput.ButtonB(), _XInput.ButtonX(), _XInput.ButtonY());
  }

  //  ���������ꂽ�u�ԂȂ�true��Ԃ�
  inline bool isReleasedAnyButtonOnXInput(const XInput& _XInput)
  {
    return isReleasedOr(_XInput.ButtonUp(), _XInput.ButtonDown(), _XInput.ButtonLeft(), _XInput.ButtonRight(),
      _XInput.ButtonStart(), _XInput.ButtonSelect(), _XInput.ButtonLeftStick(), _XInput.ButtonRightStick(),
      _XInput.ButtonLB(), _XInput.ButtonRB(), _XInput.ButtonA(), _XInput.ButtonB(), _XInput.ButtonX(), _XInput.ButtonY());
  }

  //  ����������Ă��Ȃ��Ȃ�true��Ԃ�
  inline bool isReleaseAnyButtonOnXInput(const XInput& _XInput)
  {
    return isReleaseOr(_XInput.ButtonUp(), _XInput.ButtonDown(), _XInput.ButtonLeft(), _XInput.ButtonRight(),
      _XInput.ButtonStart(), _XInput.ButtonSelect(), _XInput.ButtonLeftStick(), _XInput.ButtonRightStick(),
      _XInput.ButtonLB(), _XInput.ButtonRB(), _XInput.ButtonA(), _XInput.ButtonB(), _XInput.ButtonX(), _XInput.ButtonY());
  }
}
