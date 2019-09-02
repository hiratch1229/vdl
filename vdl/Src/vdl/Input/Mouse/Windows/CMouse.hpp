#pragma once
#include "../IMouse.hpp"

#include <vdl/Input/InputState.hpp>

#include <mutex>

class IKeyboard;

class CMouse : public IMouse
{
  //  使用ボタン数
  static constexpr int kButtonNum = 5;
private:
  IKeyboard* pKeyboard_;
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
  bool isWithinRange(int _Number)const
  {
    return (0 <= _Number && _Number < kButtonNum);
  }
public:
  CMouse() = default;

  void Initialize()override;

  void Update()override;

  bool Press(int _Number)const override
  {
    return isWithinRange(_Number) ? InputStatus_[_Number].Press() : false;
  }

  bool Pressed(int _Number)const override
  {
    return isWithinRange(_Number) ? InputStatus_[_Number].Pressed() : false;
  }

  bool Released(int _Number)const override
  {
    return isWithinRange(_Number) ? InputStatus_[_Number].Released() : false;
  }

  bool Release(int _Number)const override
  {
    return isWithinRange(_Number) ? InputStatus_[_Number].Release() : false;
  }

  vdl::int2 GetPos()const override
  {
    return Pos_;
  }

  vdl::int2 GetDelta()const override
  {
    return Delta_;
  }

  vdl::int2 GetWheel()const override
  {
    return Scroll_;
  }

  void SetPos(vdl::int2 _Pos)override;

  void Scroll(vdl::int2 _Scroll)override;
};
