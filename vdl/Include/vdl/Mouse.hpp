#pragma once
#include "Type2.hpp"
#include "Button.hpp"

namespace vdl::Input
{
  namespace Mouse
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
    void SetPos(const int2& _Pos);

    //  �ړ��ʂ��擾
    int2 GetDelta();

    //  �z�C�[���ړ��ʂ��擾
    //  ���̐��F��,���̐��F��O
    int2 GetWheel();
  }

  //  ����������Ă���Ȃ�true��Ԃ�
  inline bool isPressAnyButtonOnMouse()
  {
    return isPressOr(Mouse::ButtonLeft, Mouse::ButtonCenter, Mouse::ButtonRight, Mouse::ButtonX1, Mouse::ButtonX2);
  }

  //  ���������ꂽ�u�ԂȂ�true��Ԃ�
  inline bool isPressedAnyButtonOnMouse()
  {
    return isPressedOr(Mouse::ButtonLeft, Mouse::ButtonCenter, Mouse::ButtonRight, Mouse::ButtonX1, Mouse::ButtonX2);
  }

  //  ���������ꂽ�u�ԂȂ�true��Ԃ�
  inline bool isReleasedAnyButtonOnMouse()
  {
    return isReleasedOr(Mouse::ButtonLeft, Mouse::ButtonCenter, Mouse::ButtonRight, Mouse::ButtonX1, Mouse::ButtonX2);
  }

  //  ����������Ă��Ȃ��Ȃ�true��Ԃ�
  inline bool isReleaseAnyButtonOnMouse()
  {
    return isReleaseOr(Mouse::ButtonLeft, Mouse::ButtonCenter, Mouse::ButtonRight, Mouse::ButtonX1, Mouse::ButtonX2);
  }
}
