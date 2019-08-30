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

  //  左 ボタン
  constexpr Button ButtonLeft{ InputDevice::eMouse, Buttons::eLeft };
  //  中(ホイール) ボタン
  constexpr Button ButtonCenter{ InputDevice::eMouse, Buttons::eMiddle };
  //  右 ボタン
  constexpr Button ButtonRight{ InputDevice::eMouse, Buttons::eRight };
  //  X1(戻る) ボタン
  constexpr Button ButtonX1{ InputDevice::eMouse, Buttons::eX1 };
  //  X2(進む) ボタン
  constexpr Button ButtonX2{ InputDevice::eMouse, Buttons::eX2 };

  //  座標を取得
  int2 GetPos();
  //  座標を変更
  void SetPos(int2 _Pos);
  //  移動量を取得
  int2 GetDelta();
  //  ホイール移動量を取得
  //  正の数：奥,負の数：手前
  int2 GetWheel();

  //  何か押されているならtrueを返す
  inline bool AnyButtonPress()
  {
    return PressOr(ButtonLeft, ButtonCenter, ButtonRight, ButtonX1, ButtonX2);
  }

  //  何か押された瞬間ならtrueを返す
  inline bool AnyButtonPressed()
  {
    return PressedOr(ButtonLeft, ButtonCenter, ButtonRight, ButtonX1, ButtonX2);
  }

  //  何か離された瞬間ならtrueを返す
  inline bool AnyButtonReleased()
  {
    return ReleasedOr(ButtonLeft, ButtonCenter, ButtonRight, ButtonX1, ButtonX2);
  }

  //  何か押されていないならtrueを返す
  inline bool AnyButtonRelease()
  {
    return ReleaseOr(ButtonLeft, ButtonCenter, ButtonRight, ButtonX1, ButtonX2);
  }
}
