#pragma once
#include "Types.hpp"

#include <utility>

namespace vdl::Input
{
  enum class InputDevice : uint8_t
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
        //  入力デバイス
        InputDevice InputDevice_;
        //  決められた番号
        uint8_t Code_;
        //  入力デバイスのインデックス
        uint8_t Index_;
      };
      uint32_t ID_;
    };
  public:
    Button()noexcept
      : InputDevice_(InputDevice::eNone) {}

    constexpr Button(InputDevice _InputDevice, uint8_t _Code, uint8_t _Index = 0)noexcept
      : InputDevice_(_InputDevice), Code_(_Code), Index_(_Index) {}
  public:
    [[nodiscard]] constexpr bool operator==(const Button& _Button)const noexcept { return ID_ == _Button.ID_; }

    [[nodiscard]] constexpr bool operator!=(const Button& _Button)const noexcept { return ID_ != _Button.ID_; }
  public:
    [[nodiscard]] constexpr InputDevice GetInputDevice()const noexcept { return InputDevice_; }

    [[nodiscard]] constexpr uint8_t GetCode()const noexcept { return Code_; }

    [[nodiscard]] constexpr uint8_t GetIndex()const noexcept { return Index_; }
  };

  //  押されているならtrueを返す
  [[nodiscard]] bool isPress(const Button& _Button);

  //  1つでも押されているならtrueを返す
  [[nodiscard]] inline bool isPressOr() { return false; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isPressOr(Head&& _Head, Tail&&... _Tail) { return isPress(_Head) ? true : isPressOr(std::forward<Tail>(_Tail)...); }

  //  全て押されているならtrueを返す
  [[nodiscard]] inline bool isPressAnd() { return true; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isPressAnd(Head&& _Head, Tail&&... _Tail) { return isPress(_Head) ? isPressAnd(std::forward<Tail>(_Tail)...) : false; }

  //  押された瞬間ならtrueを返す
  [[nodiscard]] bool isPressed(const Button& _Button);

  //  1つでも押された瞬間ならtrueを返す
  [[nodiscard]] inline bool isPressedOr() { return false; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isPressedOr(Head&& _Head, Tail&&... _Tail) { return isPressed(_Head) ? true : isPressedOr(std::forward<Tail>(_Tail)...); }

  //  全て押された瞬間ならtrueを返す
  [[nodiscard]] inline bool isPressedAnd() { return true; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isPressedAnd(Head&& _Head, Tail&&... _Tail) { return isPressed(_Head) ? isPressedAnd(std::forward<Tail>(_Tail)...) : false; }

  //  離された瞬間ならtrueを返す
  [[nodiscard]] bool isReleased(const Button& _Button);

  //  1つでも離された瞬間ならtrueを返す
  [[nodiscard]] inline bool isReleasedOr() { return false; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isReleasedOr(Head&& _Head, Tail&&... _Tail) { return isReleased(_Head) ? true : isReleasedOr(std::forward<Tail>(_Tail)...); }

  //  全て離された瞬間ならtrueを返す
  [[nodiscard]] inline bool isReleasedAnd() { return true; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isReleasedAnd(Head&& _Head, Tail&&... _Tail) { return isReleased(_Head) ? isReleasedAnd(std::forward<Tail>(_Tail)...) : false; }

  //  押されていないならtrueを返す
  [[nodiscard]] bool isRelease(const Button& _Button);

  //  1つでも押されていないならtrueを返す
  [[nodiscard]] inline bool isReleaseOr() { return false; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isReleaseOr(Head&& _Head, Tail&&... _Tail) { return isRelease(_Head) ? true : isReleaseOr(std::forward<Tail>(_Tail)...); }

  //  全て押されていないならtrueを返す
  [[nodiscard]] inline bool isReleaseAnd() { return true; }
  template<class Head, class... Tail>
  [[nodiscard]] inline bool isReleaseAnd(Head&& _Head, Tail&&... _Tail) { return isRelease(_Head) ? isReleaseAnd(std::forward<Tail>(_Tail)...) : false; }
}
