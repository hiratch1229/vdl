#pragma once
#include "../IGamepad.hpp"

#include <vdl/Constants/Constants.hpp>

#include <vdl/Input/InputState.hpp>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>
#include <vector>
#include <memory>

class IXInput;

class CGamepad : public IGamepad
{
  enum class PovDirection
  {
    Up,
    Down,
    Left,
    Right,

    Num
  };
private:
  static constexpr vdl::uint kPovDirectionNum = static_cast<vdl::uint>(PovDirection::Num);
  static constexpr vdl::uint kButtonNum = 32 + kPovDirectionNum;
  static constexpr long kStickMaxValue = 10000;
  static constexpr long kTriggerMaxValue = 65535;
private:
  struct Status
  {
    //  接続状態
    bool isConnect = false;
    //  入力状態
    InputState* InputStatus;
    //  左のアナログスティックの入力状態
    vdl::float2 LeftStick;
    //  右のアナログスティックの入力状態
    vdl::float2 RightStick;
    //  左のトリガーの入力状態
    float LeftTrigger;
    //  右のトリガーの入力状態
    float RightTrigger;
    //  ボタン数
    vdl::uint ButtonNum;
  };
  struct DirectInputData
  {
    //  ジョイスティック
    LPDIRECTINPUTDEVICE8 pJoyStick;
    //  振動エフェクト
    LPDIRECTINPUTEFFECT pEffect;
  };
private:
  IXInput* pXInput_;
  HWND hWnd_;
  Microsoft::WRL::ComPtr<IDirectInput8W> pDirectInput_;
  std::unique_ptr<DirectInputData[]> pDirectInputDatas_;
  std::vector<long> XInputList_;
  std::vector<vdl::uint> JoyIDList_;
private:
  Status Status_[Constants::kMaxController];
  //  DirectInputの数
  vdl::uint DirectInputNum_;
  //  XInputの数
  vdl::uint XInputNum_;
  //  コントローラの数
  vdl::uint ControllerNum_;
  //  現在のジョイスティック数
  vdl::uint DeviceNum_;
  //  作成したデバイス数
  vdl::uint CreateDeviceNum_;
private:
  bool isWithinRange(vdl::uint _Index)const
  {
    return (0 <= _Index && _Index < Constants::kMaxController && Status_[_Index].isConnect);
  }
  bool isWithinRange(vdl::uint _Number, vdl::uint _Index)const
  {
    return (0 <= _Index && _Index < Constants::kMaxController && Status_[_Index].isConnect && 0 <= _Number && _Number < Status_[_Index].ButtonNum);
  }
public:
  void CreateDirectInputDevice(const GUID& GuidProductFromDirectInput);
public:
  CGamepad() = default;

  ~CGamepad();

  void Initialize()override;

  void Update()override;

  bool Press(vdl::uint _Number, vdl::uint _Index)const override
  {
    return isWithinRange(_Number, _Index) ? Status_[_Index].InputStatus[_Number].Press() : false;
  }

  bool Pressed(vdl::uint _Number, vdl::uint _Index)const override
  {
    return isWithinRange(_Number, _Index) ? Status_[_Index].InputStatus[_Number].Pressed() : false;
  }

  bool Released(vdl::uint _Number, vdl::uint _Index)const override
  {
    return isWithinRange(_Number, _Index) ? Status_[_Index].InputStatus[_Number].Released() : false;
  }

  bool Release(vdl::uint _Number, vdl::uint _Index)const override
  {
    return isWithinRange(_Number, _Index) ? Status_[_Index].InputStatus[_Number].Release() : false;
  }

  bool isConnect(vdl::uint _Index)const override
  {
    return isWithinRange(_Index) ? Status_[_Index].isConnect : false;
  }

  int GetButtonNum(vdl::uint _Index)const override
  {
    return isWithinRange(_Index) ? Status_[_Index].ButtonNum : 0;
  }

  bool AnyButtonPress(vdl::uint _Index)const override
  {
    if (!(isWithinRange(_Index))) return false;

    for (int i = 0; i < kButtonNum; ++i)
    {
      if (Press(i, _Index))
      {
        return true;
      }
    }

    //  何も押されていない
    return false;
  }

  bool AnyButtonPressed(vdl::uint _Index)const override
  {
    if (!(isWithinRange(_Index))) return false;

    for (int i = 0; i < kButtonNum; ++i)
    {
      if (Pressed(i, _Index))
      {
        return true;
      }
    }

    //  何も押されていない
    return false;
  }

  bool AnyButtonReleased(vdl::uint _Index)const override
  {
    if (!(isWithinRange(_Index))) return false;

    for (int i = 0; i < kButtonNum; ++i)
    {
      if (Released(i, _Index))
      {
        return true;
      }
    }

    //  何も押されていない
    return false;
  }

  vdl::input::Button GetButton(vdl::uint _Index, vdl::uint _Number)const
  {
    return isWithinRange(_Index) ? vdl::input::Button(vdl::input::InputDevice::eGamepad, _Number, _Index) : vdl::input::Button();
  }

  vdl::float2 GetLeftStick(vdl::uint _Index, float _DeadZone)const override;

  vdl::float2 GetRightStick(vdl::uint _Index, float _DeadZone)const override;

  float GetLeftTrigger(vdl::uint _Index, float _DeadZone)const override;

  float GetRightTrigger(vdl::uint _Index, float _DeadZone)const override;

  void SetVibration(vdl::uint _Index, float _Speed)const override;

  void StopVibration(vdl::uint _Index)const override;
};
