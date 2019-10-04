#pragma once
#include "Fwd.hpp"

namespace vdl::Input
{
  enum class InputDeviceType : uint8_t
  {
    eNone,
    eKeyboard,
    eMouse,
    eXInput,
    eGamepad,
  };

  class Button
  {
    union
    {
      struct
      {
        //  ���̓f�o�C�X
        InputDeviceType InputDevice_;
        //  ���߂�ꂽ�ԍ�
        uint8_t Code_;
        //  ���̓f�o�C�X�̃C���f�b�N�X
        uint8_t Index_;
      };
      uint32_t ID_;
    };
  public:
    Button()noexcept
      : InputDevice_(InputDeviceType::eNone) {}

    constexpr Button(InputDeviceType _InputDevice, uint8_t _Code, uint8_t _Index = 0)noexcept
      : InputDevice_(_InputDevice), Code_(_Code), Index_(_Index) {}
  public:
    [[nodiscard]] constexpr bool operator==(const Button& _Button)const noexcept { return ID_ == _Button.ID_; }

    [[nodiscard]] constexpr bool operator!=(const Button& _Button)const noexcept { return ID_ != _Button.ID_; }
  public:
    [[nodiscard]] constexpr InputDeviceType GetInputDevice()const noexcept { return InputDevice_; }

    [[nodiscard]] constexpr uint8_t GetCode()const noexcept { return Code_; }

    [[nodiscard]] constexpr uint8_t GetIndex()const noexcept { return Index_; }
  };

  //  ������Ă���Ȃ�true��Ԃ�
  [[nodiscard]] bool isPress(const Button& _Button);

  //  1�ł�������Ă���Ȃ�true��Ԃ�
  [[nodiscard]] inline bool isPressOr() { return false; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isPressOr(Head&& _Head, Tail&&... _Tail) { return isPress(_Head) ? true : isPressOr(std::forward<Tail>(_Tail)...); }

  //  �S�ĉ�����Ă���Ȃ�true��Ԃ�
  [[nodiscard]] inline bool isPressAnd() { return true; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isPressAnd(Head&& _Head, Tail&&... _Tail) { return isPress(_Head) ? isPressAnd(std::forward<Tail>(_Tail)...) : false; }

  //  �����ꂽ�u�ԂȂ�true��Ԃ�
  [[nodiscard]] bool isPressed(const Button& _Button);

  //  1�ł������ꂽ�u�ԂȂ�true��Ԃ�
  [[nodiscard]] inline bool isPressedOr() { return false; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isPressedOr(Head&& _Head, Tail&&... _Tail) { return isPressed(_Head) ? true : isPressedOr(std::forward<Tail>(_Tail)...); }

  //  �S�ĉ����ꂽ�u�ԂȂ�true��Ԃ�
  [[nodiscard]] inline bool isPressedAnd() { return true; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isPressedAnd(Head&& _Head, Tail&&... _Tail) { return isPressed(_Head) ? isPressedAnd(std::forward<Tail>(_Tail)...) : false; }

  //  �����ꂽ�u�ԂȂ�true��Ԃ�
  [[nodiscard]] bool isReleased(const Button& _Button);

  //  1�ł������ꂽ�u�ԂȂ�true��Ԃ�
  [[nodiscard]] inline bool isReleasedOr() { return false; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isReleasedOr(Head&& _Head, Tail&&... _Tail) { return isReleased(_Head) ? true : isReleasedOr(std::forward<Tail>(_Tail)...); }

  //  �S�ė����ꂽ�u�ԂȂ�true��Ԃ�
  [[nodiscard]] inline bool isReleasedAnd() { return true; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isReleasedAnd(Head&& _Head, Tail&&... _Tail) { return isReleased(_Head) ? isReleasedAnd(std::forward<Tail>(_Tail)...) : false; }

  //  ������Ă��Ȃ��Ȃ�true��Ԃ�
  [[nodiscard]] bool isRelease(const Button& _Button);

  //  1�ł�������Ă��Ȃ��Ȃ�true��Ԃ�
  [[nodiscard]] inline bool isReleaseOr() { return false; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isReleaseOr(Head&& _Head, Tail&&... _Tail) { return isRelease(_Head) ? true : isReleaseOr(std::forward<Tail>(_Tail)...); }

  //  �S�ĉ�����Ă��Ȃ��Ȃ�true��Ԃ�
  [[nodiscard]] inline bool isReleaseAnd() { return true; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isReleaseAnd(Head&& _Head, Tail&&... _Tail) { return isRelease(_Head) ? isReleaseAnd(std::forward<Tail>(_Tail)...) : false; }
}
