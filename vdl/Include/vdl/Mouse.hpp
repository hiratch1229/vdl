#pragma once
#include "Type2.hpp"
#include "Button.hpp"

namespace vdl::input::mouse
{
  enum Buttons
  {
    eLeft,
    eMiddle,
    eRight,
    eX1,
    eX2
  };

  //  �� �{�^��
  constexpr Button ButtonLeft{ InputDevice::eMouse, Buttons::eLeft };
  //  ��(�z�C�[��) �{�^��
  constexpr Button ButtonCenter{ InputDevice::eMouse, Buttons::eMiddle };
  //  �E �{�^��
  constexpr Button ButtonRight{ InputDevice::eMouse, Buttons::eRight };
  //  X1(�߂�) �{�^��
  constexpr Button ButtonX1{ InputDevice::eMouse, Buttons::eX1 };
  //  X2(�i��) �{�^��
  constexpr Button ButtonX2{ InputDevice::eMouse, Buttons::eX2 };

  //  ���W���擾
  int2 GetPos();
  //  ���W��ύX
  void SetPos(int2 _Pos);
  //  �ړ��ʂ��擾
  int2 GetDelta();
  //  �z�C�[���ړ��ʂ��擾
  //  ���̐��F��,���̐��F��O
  int2 GetWheel();

  //  ����������Ă���Ȃ�true��Ԃ�
  inline bool AnyButtonPress()
  {
    return PressOr(ButtonLeft, ButtonCenter, ButtonRight, ButtonX1, ButtonX2);
  }

  //  ���������ꂽ�u�ԂȂ�true��Ԃ�
  inline bool AnyButtonPressed()
  {
    return PressedOr(ButtonLeft, ButtonCenter, ButtonRight, ButtonX1, ButtonX2);
  }

  //  ���������ꂽ�u�ԂȂ�true��Ԃ�
  inline bool AnyButtonReleased()
  {
    return ReleasedOr(ButtonLeft, ButtonCenter, ButtonRight, ButtonX1, ButtonX2);
  }

  //  ����������Ă��Ȃ��Ȃ�true��Ԃ�
  inline bool AnyButtonRelease()
  {
    return ReleaseOr(ButtonLeft, ButtonCenter, ButtonRight, ButtonX1, ButtonX2);
  }
}
