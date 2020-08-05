#pragma once
#include "Fwd.hpp"
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
    inline constexpr Button ButtonLeft{ InputDeviceType::eMouse, Buttons::eLeft };
    //  ��(�z�C�[��) �{�^��
    inline constexpr Button ButtonCenter{ InputDeviceType::eMouse, Buttons::eMiddle };
    //  �E �{�^��
    inline constexpr Button ButtonRight{ InputDeviceType::eMouse, Buttons::eRight };
    //  X1(�߂�) �{�^��
    inline constexpr Button ButtonX1{ InputDeviceType::eMouse, Buttons::eX1 };
    //  X2(�i��) �{�^��
    inline constexpr Button ButtonX2{ InputDeviceType::eMouse, Buttons::eX2 };

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
  [[nodiscard]] inline bool isPressAnyButtonOnMouse()
  {
    return isPressOr(Mouse::ButtonLeft, Mouse::ButtonCenter, Mouse::ButtonRight, Mouse::ButtonX1, Mouse::ButtonX2);
  }

  //  ���������ꂽ�u�ԂȂ�true��Ԃ�
  [[nodiscard]] inline bool isPressedAnyButtonOnMouse()
  {
    return isPressedOr(Mouse::ButtonLeft, Mouse::ButtonCenter, Mouse::ButtonRight, Mouse::ButtonX1, Mouse::ButtonX2);
  }

  //  ���������ꂽ�u�ԂȂ�true��Ԃ�
  [[nodiscard]] inline bool isReleasedAnyButtonOnMouse()
  {
    return isReleasedOr(Mouse::ButtonLeft, Mouse::ButtonCenter, Mouse::ButtonRight, Mouse::ButtonX1, Mouse::ButtonX2);
  }

  //  ����������Ă��Ȃ��Ȃ�true��Ԃ�
  [[nodiscard]] inline bool isReleaseAnyButtonOnMouse()
  {
    return isReleaseOr(Mouse::ButtonLeft, Mouse::ButtonCenter, Mouse::ButtonRight, Mouse::ButtonX1, Mouse::ButtonX2);
  }
}
