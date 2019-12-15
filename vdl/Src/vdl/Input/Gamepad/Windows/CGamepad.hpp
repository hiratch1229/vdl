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
    //  �ڑ����
    bool isConnect = false;
    //  ���͏��
    InputState* InputStatus;
    //  ���̃A�i���O�X�e�B�b�N�̓��͏��
    vdl::float2 LeftStick;
    //  �E�̃A�i���O�X�e�B�b�N�̓��͏��
    vdl::float2 RightStick;
    //  ���̃g���K�[�̓��͏��
    float LeftTrigger;
    //  �E�̃g���K�[�̓��͏��
    float RightTrigger;
    //  �{�^����
    vdl::uint ButtonNum;
  };
  struct DirectInputData
  {
    //  �W���C�X�e�B�b�N
    LPDIRECTINPUTDEVICE8 pJoyStick;
    //  �U���G�t�F�N�g
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
  //  DirectInput�̐�
  vdl::uint DirectInputNum_;
  //  XInput�̐�
  vdl::uint XInputNum_;
  //  �R���g���[���̐�
  vdl::uint ControllerNum_;
  //  ���݂̃W���C�X�e�B�b�N��
  vdl::uint DeviceNum_;
  //  �쐬�����f�o�C�X��
  vdl::uint CreateDeviceNum_;
private:
  bool isWithinRange(vdl::uint _Index)const
  {
    return (0 <= _Index && _Index < Constants::kMaxController && Status_[_Index].isConnect);
  }
  bool isWithinRange(vdl::uint _Index, vdl::uint _Code)const
  {
    return (0 <= _Index && _Index < Constants::kMaxController && Status_[_Index].isConnect && 0 <= _Code && _Code < Status_[_Index].ButtonNum);
  }
public:
  void CreateDirectInputDevice(const GUID& GuidProductFromDirectInput);
public:
  CGamepad() = default;

  ~CGamepad();

  PlatformType GetPlatform()const final { return PlatformType::eWindows; }

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

  vdl::uint GetButtonNum(vdl::uint _Index)const override
  {
    return isWithinRange(_Index) ? Status_[_Index].ButtonNum : 0;
  }

  vdl::float2 GetLeftStick(vdl::uint _Index, float _DeadZone)const override;

  vdl::float2 GetRightStick(vdl::uint _Index, float _DeadZone)const override;

  float GetLeftTrigger(vdl::uint _Index, float _DeadZone)const override;

  float GetRightTrigger(vdl::uint _Index, float _DeadZone)const override;

  void SetVibration(vdl::uint _Index, float _Speed)const override;

  void StopVibration(vdl::uint _Index)const override;
};
