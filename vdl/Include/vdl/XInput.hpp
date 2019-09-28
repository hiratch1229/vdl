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

  //  接続されているか確認
  //  接続中はtrue
  [[nodiscard]] bool isConnect(const XInput& _XInput);

  //  左スティックの入力状態を取得
  //  -1~1の範囲で表現されます
  [[nodiscard]] float2 GetLeftStick(const XInput& _XInput, float _DeadZone = 0.0f);

  //  右スティックの入力状態を取得
  //  -1~1の範囲で表現されます
  [[nodiscard]] float2 GetRightStick(const XInput& _XInput, float _DeadZone = 0.0f);

  //  左トリガーの入力状態を取得
  //  0-1の範囲で表現されます
  [[nodiscard]] float GetLeftTrigger(const XInput& _XInput, float _DeadZone = 0.0f);

  //  右トリガーの入力状態を取得
  //  0-1の範囲で表現されます
  [[nodiscard]] float GetRightTrigger(const XInput& _XInput, float _DeadZone = 0.0f);
  
  //  コントローラを振動させます
  //  0は停止、1は最大速度
  void SetVibration(const XInput& _XInput, float _Speed);

  //  コントローラの振動を止めます
  void StopVibration(const XInput& _XInput);

  //  何か押されているならtrueを返す
  inline bool isPressAnyButtonOnXInput(const XInput& _XInput)
  {
    return isPressOr(_XInput.ButtonUp(), _XInput.ButtonDown(), _XInput.ButtonLeft(), _XInput.ButtonRight(),
      _XInput.ButtonStart(), _XInput.ButtonSelect(), _XInput.ButtonLeftStick(), _XInput.ButtonRightStick(),
      _XInput.ButtonLB(), _XInput.ButtonRB(), _XInput.ButtonA(), _XInput.ButtonB(), _XInput.ButtonX(), _XInput.ButtonY());
  }

  //  何か押された瞬間ならtrueを返す
  inline bool isPressedAnyButtonOnXInput(const XInput& _XInput)
  {
    return isPressedOr(_XInput.ButtonUp(), _XInput.ButtonDown(), _XInput.ButtonLeft(), _XInput.ButtonRight(),
      _XInput.ButtonStart(), _XInput.ButtonSelect(), _XInput.ButtonLeftStick(), _XInput.ButtonRightStick(),
      _XInput.ButtonLB(), _XInput.ButtonRB(), _XInput.ButtonA(), _XInput.ButtonB(), _XInput.ButtonX(), _XInput.ButtonY());
  }

  //  何か離された瞬間ならtrueを返す
  inline bool isReleasedAnyButtonOnXInput(const XInput& _XInput)
  {
    return isReleasedOr(_XInput.ButtonUp(), _XInput.ButtonDown(), _XInput.ButtonLeft(), _XInput.ButtonRight(),
      _XInput.ButtonStart(), _XInput.ButtonSelect(), _XInput.ButtonLeftStick(), _XInput.ButtonRightStick(),
      _XInput.ButtonLB(), _XInput.ButtonRB(), _XInput.ButtonA(), _XInput.ButtonB(), _XInput.ButtonX(), _XInput.ButtonY());
  }

  //  何か押されていないならtrueを返す
  inline bool isReleaseAnyButtonOnXInput(const XInput& _XInput)
  {
    return isReleaseOr(_XInput.ButtonUp(), _XInput.ButtonDown(), _XInput.ButtonLeft(), _XInput.ButtonRight(),
      _XInput.ButtonStart(), _XInput.ButtonSelect(), _XInput.ButtonLeftStick(), _XInput.ButtonRightStick(),
      _XInput.ButtonLB(), _XInput.ButtonRB(), _XInput.ButtonA(), _XInput.ButtonB(), _XInput.ButtonX(), _XInput.ButtonY());
  }
}
