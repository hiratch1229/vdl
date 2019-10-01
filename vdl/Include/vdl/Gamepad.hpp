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

    [[nodiscard]] Button GetButton(uint8_t _Number)const noexcept { return { InputDevice::eGamepad, _Number, Index_ }; }

    [[nodiscard]] uint GetButtonNum()const;
  };

  //  接続されているか確認
  //  接続中はtrue
  [[nodiscard]] bool isConnect(const Gamepad& _Gamepad);

  //  左スティックの入力状態を取得
  //  -1~1の範囲で表現されます
  [[nodiscard]] float2 GetLeftStick(const Gamepad& _Gamepad, float _DeadZone = 0.0f);

  //  右スティックの入力状態を取得
  //  -1~1の範囲で表現されます
  [[nodiscard]] float2 GetRightStick(const Gamepad& _Gamepad, float _DeadZone = 0.0f);

  //  左トリガーの入力状態を取得
  //  0-1の範囲で表現されます
  [[nodiscard]] float GetLeftTrigger(const Gamepad& _Gamepad, float _DeadZone = 0.0f);

  //  右トリガーの入力状態を取得
  //  0-1の範囲で表現されます
  [[nodiscard]] float GetRightTrigger(const Gamepad& _Gamepad, float _DeadZone = 0.0f);

  //  コントローラを振動させます
  //  0は停止、1は最大速度
  void SetVibration(const Gamepad& _Gamepad, float _Speed);

  //  コントローラの振動を止めます
  void StopVibration(const Gamepad& _Gamepad);

  //  何か押されているならtrueを返す
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

  //  何か押された瞬間ならtrueを返す
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

  //  何か離された瞬間ならtrueを返す
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

  //  何か押されていないならtrueを返す
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
