#pragma once
#include "Types.hpp"

#include <utility>

namespace vdl::input
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
    [[nodiscard]] constexpr InputDevice GetInputDevice()const noexcept { return InputDevice_; }

    [[nodiscard]] constexpr uint8_t GetCode()const noexcept { return Code_; }

    [[nodiscard]] constexpr uint8_t GetIndex()const noexcept { return Index_; }
  public:
    [[nodiscard]] constexpr bool operator==(const Button& _Button)const noexcept { return ID_ == _Button.ID_; }

    [[nodiscard]] constexpr bool operator!=(const Button& _Button)const noexcept { return !(*this == _Button); }
  public:
    Button() = default;

    constexpr Button(InputDevice _InputDevice, uint8_t _Code, uint8_t _Index = 0)noexcept
      : InputDevice_(_InputDevice), Code_(_Code), Index_(_Index) {}
  };

  //  押されているならtrueを返す
  bool Press(const Button& _Button);

  //  1つでも押されているならtrueを返す
  template<class Head, class... Tail>
  inline bool PressOr(Head&& _Head, Tail&&... _Tail) { return Press(_Head) ? true : PressOr(std::forward<Tail>(_Tail)...); }
  inline bool PressOr() { return false; }

  //  全て押されているならtrueを返す
  template<class Head, class... Tail>
  inline bool PressAnd(Head&& _Head, Tail&&... _Tail) { return Press(_Head) ? PressAnd(std::forward<Tail>(_Tail)...) : false; }
  inline bool PressAnd() { return false; }

  //  押された瞬間ならtrueを返す
  bool Pressed(const Button& _Button);

  //  1つでも押された瞬間ならtrueを返す
  template<class Head, class... Tail>
  inline bool PressedOr(Head&& _Head, Tail&&... _Tail) { return Pressed(_Head) ? true : PressedOr(std::forward<Tail>(_Tail)...); }
  inline bool PressedOr() { return false; }

  //  全て押された瞬間ならtrueを返す
  template<class Head, class... Tail>
  inline bool PressedAnd(Head&& _Head, Tail&&... _Tail) { return Pressed(_Head) ? PressedAnd(std::forward<Tail>(_Tail)...) : false; }
  inline bool PressedAnd() { return false; }

  //  離された瞬間ならtrueを返す
  bool Released(const Button& _Button);

  //  1つでも離された瞬間ならtrueを返す
  template<class Head, class... Tail>
  inline bool ReleasedOr(Head&& _Head, Tail&&... _Tail) { return Released(_Head) ? true : ReleasedOr(std::forward<Tail>(_Tail)...); }
  inline bool ReleasedOr() { return false; }

  //  全て離された瞬間ならtrueを返す
  template<class Head, class... Tail>
  inline bool ReleasedAnd(Head&& _Head, Tail&&... _Tail) { return Released(_Head) ? ReleasedAnd(std::forward<Tail>(_Tail)...) : false; }
  inline bool ReleasedAnd() { return false; }

  //  押されていないならtrueを返す
  bool Release(const Button& _Button);

  //  1つでも押されていないならtrueを返す
  template<class Head, class... Tail>
  inline bool ReleaseOr(Head&& _Head, Tail&&... _Tail) { return Release(_Head) ? true : ReleaseOr(std::forward<Tail>(_Tail)...); }
  inline bool ReleaseOr() { return false; }

  //  全て押されていないならtrueを返す
  template<class Head, class... Tail>
  inline bool ReleaseAnd(Head&& _Head, Tail&&... _Tail) { return Release(_Head) ? ReleaseAnd(std::forward<Tail>(_Tail)...) : false; }
  inline bool ReleaseAnd() { return false; }
}
