#pragma once
#include "../IKeyboard.hpp"

#include <vdl/Input/InputState.hpp>

class CKeyboard : public IKeyboard
{
  //  使用キー数
  static constexpr int kKeyNum = 256;
private:
  InputState InputStatus_[kKeyNum];
private:
  bool isWithinRange(int _Number)const
  {
    return (0 <= _Number && _Number < kKeyNum);
  }
public:
  CKeyboard() = default;

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

  bool AnyKeyPress()const override
  {
    for (int i = 0; i < kKeyNum; ++i)
    {
      if (Press(i))
      {
        return true;
      }
    }

    //  何も押されていない
    return false;
  }

  bool AnyKeyPressed()const override
  {
    for (int i = 0; i < kKeyNum; ++i)
    {
      if (Pressed(i))
      {
        return true;
      }
    }

    //  何も押されていない
    return false;
  }

  bool AnyKeyReleased()const override
  {
    for (int i = 0; i < kKeyNum; ++i)
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
