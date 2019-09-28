#pragma once
#include "../IKeyboard.hpp"

#include <vdl/Input/InputState.hpp>

class CKeyboard : public IKeyboard
{
  //  使用キー数
  static constexpr vdl::uint kKeyNum = 256;
private:
  bool isInputAttach_ = false;
  InputState InputStatus_[kKeyNum];
private:
  bool isWithinRange(vdl::uint _Code)const
  {
    return (0 <= _Code && _Code < kKeyNum);
  }
public:
  CKeyboard() = default;

  void Initialize()override {}

  void Update()override;

  bool Press(vdl::uint _Code)const override
  {
    return isWithinRange(_Code) ? InputStatus_[_Code].Press() : false;
  }

  bool Pressed(vdl::uint _Code)const override
  {
    return isWithinRange(_Code) ? InputStatus_[_Code].Pressed() : false;
  }

  bool Released(vdl::uint _Code)const override
  {
    return isWithinRange(_Code) ? InputStatus_[_Code].Released() : false;
  }

  bool Release(vdl::uint _Code)const override
  {
    return isWithinRange(_Code) ? InputStatus_[_Code].Release() : false;
  }
};
