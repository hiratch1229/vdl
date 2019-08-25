#pragma once
#include "../IMouse.hpp"

#include <vdl/Input/InputState.hpp>

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
  //  ホイール移動量
  vdl::int2 Wheel_;
  //  画面解像度
  vdl::float2 DpiScale_;
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
    return Wheel_;
  }

  void SetPos(vdl::int2 _Pos)override;

  void SetWheel(vdl::int2 _Wheel)override
  {
    Wheel_ = _Wheel;
  }

  bool AnyButtonPress()const override
  {
    for (int i = 0; i < kButtonNum; ++i)
    {
      if (Press(i))
      {
        return true;
      }
    }

    //  何も押されていない
    return false;
  }

  bool AnyButtonPressed()const override
  {
    for (int i = 0; i < kButtonNum; ++i)
    {
      if (Pressed(i))
      {
        return true;
      }
    }

    //  何も押されていない
    return false;
  }

  bool AnyButtonReleased()const override
  {
    for (int i = 0; i < kButtonNum; ++i)
    {
      if (Released(i))
      {
        return true;
      }
    }

    //  何も離されていない
    return false;
  }
};
