#pragma once
#include "../IMouse.hpp"

#include <vdl/Input/InputState.hpp>

#include <vdl/pch/Windows/pch.hpp>

#include <mutex>

class IKeyboard;

class CMouse : public IMouse
{
  //  使用ボタン数
  static constexpr vdl::uint kButtonNum = 5;
private:
  IKeyboard* pKeyboard_;
  HWND hWnd_;
private:
  //  入力状態
  InputState InputStatus_[kButtonNum];
  //  座標
  vdl::int2 Pos_;
  //  移動量
  vdl::int2 Delta_;
  //  現在のホイールの移動量
  vdl::int2 Scroll_;
  //  ホイールの移動量の蓄積
  vdl::int2 AccumulationScroll_;
  //  画面解像度
  vdl::float2 DpiScale_;
  //  スクロールの排他制御
  std::mutex ScrollMutex_;
private:
  bool isWithinRange(vdl::uint _Code)const { return (0 <= _Code && _Code < kButtonNum); }
public:
  CMouse() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eWindows; }

  void Initialize()override;

  void Update()override;

  bool Press(vdl::uint _Code)const override { return isWithinRange(_Code) ? InputStatus_[_Code].Press() : false; }

  bool Pressed(vdl::uint _Code)const override { return isWithinRange(_Code) ? InputStatus_[_Code].Pressed() : false; }

  bool Released(vdl::uint _Code)const override { return isWithinRange(_Code) ? InputStatus_[_Code].Released() : false; }

  bool Release(vdl::uint _Code)const override { return isWithinRange(_Code) ? InputStatus_[_Code].Release() : false; }

  const vdl::int2& GetPos()const override { return Pos_; }

  const vdl::int2& GetDelta()const override { return Delta_; }

  const vdl::int2& GetWheel()const override { return Scroll_; }

  void SetPos(const vdl::int2& _Pos)override;

  void Scroll(const vdl::int2& _Scroll)override;

  void SetCursor(MouseCursorType _Type)override;
};
