#pragma once
#include "../IXInput.hpp"

#include <vdl/Constants/Constants.hpp>

#include <vdl/Input/InputState.hpp>

class CXInput : public IXInput
{
  static constexpr int kAnalogStickMaxValue = 65535;
  static constexpr float kTriggerMaxValue = 255.0f;
private:
  struct Status
  {
    //  接続状態
    bool isConnect;
    //  入力状態
    InputState InputStatus[Constants::kXInputButtonNum];
    //  左のアナログスティックの入力状態
    vdl::float2 LeftStick;
    //  右のアナログスティックの入力状態
    vdl::float2 RightStick;
    //  左のトリガーの入力状態
    float LeftTrigger;
    //  右のトリガーの入力状態
    float RightTrigger;
  };
private:
  Status Status_[Constants::kMaxController];
private:
  bool isWithinRange(vdl::uint _Index)const
  {
    return (Status_[_Index].isConnect && 0 <= _Index && _Index < Constants::kMaxController);
  }
  bool isWithinRange(vdl::uint _Index, vdl::uint _Code)const
  {
    return (Status_[_Index].isConnect && 0 <= _Code && _Code < Constants::kXInputButtonNum && 0 <= _Index && _Index < Constants::kMaxController);
  }
public:
  CXInput() = default;

  void Initialize()override;

  void Update()override;

  bool Press(vdl::uint _Index, vdl::uint _Code)const override
  {
    return isWithinRange(_Index, _Code) ? Status_[_Index].InputStatus[_Code].Press() : false;
  }

  bool Pressed(vdl::uint _Index, vdl::uint _Code)const override
  {
    return isWithinRange(_Index, _Code) ? Status_[_Index].InputStatus[_Code].Pressed() : false;
  }

  bool Released(vdl::uint _Index, vdl::uint _Code)const override
  {
    return isWithinRange(_Index, _Code) ? Status_[_Index].InputStatus[_Code].Released() : false;
  }

  bool Release(vdl::uint _Index, vdl::uint _Code)const override
  {
    return isWithinRange(_Index, _Code) ? Status_[_Index].InputStatus[_Code].Release() : false;
  }

  bool isConnect(vdl::uint _Index)const override
  {
    return isWithinRange(_Index) ? Status_[_Index].isConnect : false;
  }

  vdl::float2 GetLeftStick(vdl::uint _Index, float _DeadZone)const override;

  vdl::float2 GetRightStick(vdl::uint _Index, float _DeadZone)const override;

  float GetLeftTrigger(vdl::uint _Index, float _DeadZone)const override;

  float GetRightTrigger(vdl::uint _Index, float _DeadZone)const override;

  void SetVibration(vdl::uint _Index, float _Speed)const override;

  void StopVibration(vdl::uint _Index)const override;
};
