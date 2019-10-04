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

    //  左 ボタン
    constexpr Button ButtonLeft{ InputDeviceType::eMouse, Buttons::eLeft };
    //  中(ホイール) ボタン
    constexpr Button ButtonCenter{ InputDeviceType::eMouse, Buttons::eMiddle };
    //  右 ボタン
    constexpr Button ButtonRight{ InputDeviceType::eMouse, Buttons::eRight };
    //  X1(戻る) ボタン
    constexpr Button ButtonX1{ InputDeviceType::eMouse, Buttons::eX1 };
    //  X2(進む) ボタン
    constexpr Button ButtonX2{ InputDeviceType::eMouse, Buttons::eX2 };

    //  座標を取得
    int2 GetPos();

    //  座標を変更
    void SetPos(const int2& _Pos);

    //  移動量を取得
    int2 GetDelta();

    //  ホイール移動量を取得
    //  正の数：奥,負の数：手前
    int2 GetWheel();
  }

  //  何か押されているならtrueを返す
  inline bool isPressAnyButtonOnMouse()
  {
    return isPressOr(Mouse::ButtonLeft, Mouse::ButtonCenter, Mouse::ButtonRight, Mouse::ButtonX1, Mouse::ButtonX2);
  }

  //  何か押された瞬間ならtrueを返す
  inline bool isPressedAnyButtonOnMouse()
  {
    return isPressedOr(Mouse::ButtonLeft, Mouse::ButtonCenter, Mouse::ButtonRight, Mouse::ButtonX1, Mouse::ButtonX2);
  }

  //  何か離された瞬間ならtrueを返す
  inline bool isReleasedAnyButtonOnMouse()
  {
    return isReleasedOr(Mouse::ButtonLeft, Mouse::ButtonCenter, Mouse::ButtonRight, Mouse::ButtonX1, Mouse::ButtonX2);
  }

  //  何か押されていないならtrueを返す
  inline bool isReleaseAnyButtonOnMouse()
  {
    return isReleaseOr(Mouse::ButtonLeft, Mouse::ButtonCenter, Mouse::ButtonRight, Mouse::ButtonX1, Mouse::ButtonX2);
  }
}
