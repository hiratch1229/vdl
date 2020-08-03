#include "CGamepad.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Platform/IPlatform.hpp>
#include <vdl/Window/Windows/CWindow.hpp>
#include <vdl/Input/XInput/IXInput.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/Type2.hpp>
#include <vdl/Math.hpp>
#include <vdl/DetectMemoryLeak.hpp>

#include <joystickapi.h>

namespace
{
  inline BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, void*)
  {
    //  DirectInput���쐬
    //  TODO
    Cast<CGamepad>(Engine::Get<IGamepad>())->CreateDirectInputDevice(pdidInstance->guidProduct);

    //  ����
    return DIENUM_CONTINUE;
  }

  inline vdl::float2 GetStick(vdl::float2 _Value, float _DeadZone)
  {
    if (vdl::Math::Abs(_Value.x) < _DeadZone)
    {
      _Value.x = 0.0f;
    }
    if (vdl::Math::Abs(_Value.y) < _DeadZone)
    {
      _Value.y = 0.0f;
    }

    return _Value;
  }

  inline float GetTrigger(float _Value, float _DeadZone)
  {
    if (_Value < _DeadZone)
    {
      _Value = 0.0f;
    }

    return _Value;
  }
}

void CGamepad::Initialize()
{
  pXInput_ = Engine::Get<IXInput>();
  hWnd_ = Cast<CWindow>(Engine::Get<IWindow>())->GetHandle();

  HRESULT hr = S_OK;

  //  DirectInput���쐬
  hr = ::DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(pDirectInput_.GetAddressOf()), NULL);
  ERROR_CHECK(hr);

  //  Joystick���쐬
  hr = pDirectInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, nullptr, DIEDFL_ATTACHEDONLY);
  ERROR_CHECK(hr);

  //  �{�^���̐���ۑ�
  {
    for (vdl::uint i = 0; i < XInputNum_; ++i)
    {
      Status_[i].ButtonNum = Constants::kXInputButtonNum;
      Status_[i].InputStatus = new InputState[Constants::kXInputButtonNum];
    }

    JOYCAPS JoyCaps;
    for (vdl::uint i = XInputNum_; i < ControllerNum_; ++i)
    {
      joyGetDevCaps(JoyIDList_[i - XInputNum_], &JoyCaps, sizeof(JoyCaps));

      const int ButtonNum = JoyCaps.wNumButtons + kPovDirectionNum;

      Status_[i].ButtonNum = ButtonNum;
      Status_[i].InputStatus = new InputState[ButtonNum];
    }
  }
}

CGamepad::~CGamepad()
{
  for (vdl::uint i = 0; i < DirectInputNum_; ++i)
  {
    if (pDirectInputDatas_[i].pJoyStick)
    {
      pDirectInputDatas_[i].pJoyStick->Unacquire();
    }
  }

  for (vdl::uint i = 0; i < ControllerNum_; ++i)
  {
    delete[] Status_[i].InputStatus;
  }
}

void CGamepad::Update()
{
  auto XInputUpdate = [&](vdl::uint _Index)
  {
    Status& Status = Status_[_Index];

    for (int i = 0; i < Constants::kXInputButtonNum; ++i)
    {
      Status.InputStatus[i].Update(pXInput_->Press(i, _Index));
    }

    Status.isConnect = pXInput_->isConnect(_Index);
    Status.LeftStick = pXInput_->GetLeftStick(_Index, 0.0f);
    Status.RightStick = pXInput_->GetRightStick(_Index, 0.0f);
    Status.LeftTrigger = pXInput_->GetLeftTrigger(_Index, 0.0f);
    Status.RightTrigger = pXInput_->GetRightTrigger(_Index, 0.0f);
  };

  auto DirectInputUpdate = [&](vdl::uint _Index)->void
  {
    Status& Status = Status_[_Index];

    DIJOYSTATE State;

    LPDIRECTINPUTDEVICE8& pJoyStick = pDirectInputDatas_[_Index - XInputNum_].pJoyStick;
    pJoyStick->Poll();

    if (pJoyStick->GetDeviceState(sizeof(DIJOYSTATE), &State) != DI_OK)
    {
      Status.isConnect = false;
      return;
    }

    Status.isConnect = true;

    Status.LeftStick = vdl::float2(State.lX, -State.lY) / kStickMaxValue;
    Status.RightStick = vdl::float2(State.lZ, -State.lRz) / kStickMaxValue;
    Status.LeftTrigger = State.lRx / static_cast<float>(kTriggerMaxValue);
    Status.RightTrigger = State.lRy / static_cast<float>(kTriggerMaxValue);

    const int PovInput = State.rgdwPOV[0] / 4500;

    bool isInput[kPovDirectionNum]{};

    switch (PovInput)
    {
    case 0:
      isInput[static_cast<int>(PovDirection::Up)] = true;
      break;
    case 1:
      isInput[static_cast<int>(PovDirection::Up)] = true;
      isInput[static_cast<int>(PovDirection::Right)] = true;
      break;
    case 2:
      isInput[static_cast<int>(PovDirection::Right)] = true;
      break;
    case 3:
      isInput[static_cast<int>(PovDirection::Right)] = true;
      isInput[static_cast<int>(PovDirection::Down)] = true;
      break;
    case 4:
      isInput[static_cast<int>(PovDirection::Down)] = true;
      break;
    case 5:
      isInput[static_cast<int>(PovDirection::Down)] = true;
      isInput[static_cast<int>(PovDirection::Left)] = true;
      break;
    case 6:
      isInput[static_cast<int>(PovDirection::Left)] = true;
      break;
    case 7:
      isInput[static_cast<int>(PovDirection::Left)] = true;
      isInput[static_cast<int>(PovDirection::Up)] = true;
      break;
    }

    for (vdl::uint j = 0; j < kPovDirectionNum; ++j)
    {
      Status.InputStatus[j].Update(isInput[j]);
    }

    for (vdl::uint j = kPovDirectionNum; j < Status.ButtonNum; ++j)
    {
      Status.InputStatus[j].Update((State.rgbButtons[j - kPovDirectionNum] & 0x80) != 0);
    }
  };

  for (vdl::uint i = 0; i < ControllerNum_; ++i)
  {
    if (i < XInputNum_)
    {
      XInputUpdate(i);
    }
    else
    {
      DirectInputUpdate(i);
    }
  }
}

vdl::float2 CGamepad::GetLeftStick(vdl::uint _Index, float _DeadZone)const
{
  if (!isWithinRange(_Index))
  {
    return vdl::float2(0.0f, 0.0f);
  }

  vdl::float2 Value = Status_[_Index].LeftStick;
  {
    if (vdl::Math::Abs(Value.x) < _DeadZone)
    {
      Value.x = 0.0f;
    }
    if (vdl::Math::Abs(Value.y) < _DeadZone)
    {
      Value.y = 0.0f;
    }
  }

  return Value;
}

vdl::float2 CGamepad::GetRightStick(vdl::uint _Index, float _DeadZone)const
{
  if (!isWithinRange(_Index))
  {
    return vdl::float2(0.0f, 0.0f);
  }

  vdl::float2 Value = Status_[_Index].RightStick;
  {
    if (vdl::Math::Abs(Value.x) < _DeadZone)
    {
      Value.x = 0.0f;
    }
    if (vdl::Math::Abs(Value.y) < _DeadZone)
    {
      Value.y = 0.0f;
    }
  }

  return Value;
}

float CGamepad::GetLeftTrigger(vdl::uint _Index, float _DeadZone)const
{
  if (!isWithinRange(_Index))
  {
    return 0.0f;
  }

  float Value = Status_[_Index].LeftTrigger;
  {
    if (Value < _DeadZone)
    {
      Value = 0.0f;
    }
  }

  return Value;
}

float CGamepad::GetRightTrigger(vdl::uint _Index, float _DeadZone)const
{
  if (!isWithinRange(_Index))
  {
    return 0.0f;
  }

  float Value = Status_[_Index].RightTrigger;
  {
    if (Value < _DeadZone)
    {
      Value = 0.0f;
    }
  }

  return Value;
}

void CGamepad::SetVibration(vdl::uint _Index, float _Speed)const
{
  if (!isWithinRange(_Index))
  {
    return;
  }

  //  XInput��U��������
  if (_Index < XInputNum_)
  {
    pXInput_->SetVibration(_Index, _Speed);
  }

  //const int DirectInputIndex = _Index - pImpl_->XInputNum_;
  //if (pImpl_->pEffects_[DirectInputIndex]) pImpl_->pEffects_[DirectInputIndex]->Start(1, 0);
}

void CGamepad::StopVibration(vdl::uint _Index)const
{
  if (!isWithinRange(_Index))
  {
    return;
  }

  //  XInput�̐U�����~�߂�
  if (_Index < XInputNum_)
  {
    pXInput_->StopVibration(_Index);
  }

  //const int DirectInputIndex = _Index - pImpl_->XInputNum_;
  //if (pImpl_->pEffects_[DirectInputIndex]) pImpl_->pEffects_[DirectInputIndex]->Stop();
}

//--------------------------------------------------

void CGamepad::CreateDirectInputDevice(const GUID& GuidProductFromDirectInput)
{
  HRESULT hr = S_OK;

  ++DeviceNum_;

  //  XInput����
  for (vdl::uint i = 0, Size = static_cast<vdl::uint>(XInputList_.size()); i < Size; ++i)
  {
    //  XInput�ł���
    if (static_cast<long>(XInputList_[i]) == GuidProductFromDirectInput.Data1)
    {
      return;
    }
  }

  DirectInputData& pDirectInputData = pDirectInputDatas_[CreateDeviceNum_++];

  //  �W���C�X�e�B�b�N���쐬
  hr = pDirectInput_->CreateDevice(GuidProductFromDirectInput, &pDirectInputData.pJoyStick, nullptr);
  ERROR_CHECK(hr);
  hr = pDirectInputData.pJoyStick->SetDataFormat(&c_dfDIJoystick);
  ERROR_CHECK(hr);
  hr = pDirectInputData.pJoyStick->SetCooperativeLevel(hWnd_, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
  ERROR_CHECK(hr);

  // ���̒l�͈̔͂�ݒ�
  DIPROPRANGE DirectInputPropatyRange;
  {
    DirectInputPropatyRange.diph.dwSize = sizeof(DirectInputPropatyRange);
    DirectInputPropatyRange.diph.dwHeaderSize = sizeof(DirectInputPropatyRange.diph);
    DirectInputPropatyRange.diph.dwObj = DIJOFS_X;
    DirectInputPropatyRange.diph.dwHow = DIPH_BYOFFSET;
    DirectInputPropatyRange.lMin = -kStickMaxValue;
    DirectInputPropatyRange.lMax = +kStickMaxValue;
    hr = pDirectInputData.pJoyStick->SetProperty(DIPROP_RANGE, &DirectInputPropatyRange.diph);
    DirectInputPropatyRange.diph.dwObj = DIJOFS_Y;
    hr = pDirectInputData.pJoyStick->SetProperty(DIPROP_RANGE, &DirectInputPropatyRange.diph);
    DirectInputPropatyRange.diph.dwObj = DIJOFS_Z;
    hr = pDirectInputData.pJoyStick->SetProperty(DIPROP_RANGE, &DirectInputPropatyRange.diph);
    DirectInputPropatyRange.diph.dwObj = DIJOFS_RZ;
    hr = pDirectInputData.pJoyStick->SetProperty(DIPROP_RANGE, &DirectInputPropatyRange.diph);
  }

  //  �U����ݒ�
  {
    // �G�t�F�N�g�����ݒ�
    DIPERIODIC diPeriodic;
    diPeriodic.dwMagnitude = DI_FFNOMINALMAX;
    diPeriodic.lOffset = 0;
    diPeriodic.dwPhase = 0;
    diPeriodic.dwPeriod = (DWORD)(0.5f * DI_SECONDS);

    // �U���G�t�F�N�g�ݒ�
    DIEFFECT DirectInputEffect{};
    DirectInputEffect.dwSize = sizeof(DIEFFECT);
    DirectInputEffect.dwFlags = DIEFF_OBJECTOFFSETS;
    DirectInputEffect.dwDuration = 0;
    DirectInputEffect.dwSamplePeriod = 0;
    DirectInputEffect.dwGain = DI_FFNOMINALMAX;

    //  �g���K�[�ݒ�
    DirectInputEffect.dwTriggerButton = DIEB_NOTRIGGER;
    DirectInputEffect.dwTriggerRepeatInterval = 0;
    //  �G�t�F�N�g���ݒ�
    DirectInputEffect.dwFlags |= DIEFF_POLAR;
    DWORD Axes[] = { DIJOFS_X, DIJOFS_Y };  // �G�t�F�N�g��
    LONG Direction[] = { 1, 1 };            // �G�t�F�N�g����
    DirectInputEffect.cAxes = 2;
    DirectInputEffect.rgdwAxes = Axes;
    DirectInputEffect.rglDirection = Direction;
    //  �����ݒ�
    DirectInputEffect.lpEnvelope = nullptr; // �G���x���[�v�\����
    DirectInputEffect.cbTypeSpecificParams = sizeof(diPeriodic); // �G�t�F�N�g�����\���̂̃T�C�Y
    DirectInputEffect.lpvTypeSpecificParams = &diPeriodic; // �G�t�F�N�g�����\����

    pDirectInputData.pJoyStick->CreateEffect(GUID_Square, &DirectInputEffect, &pDirectInputData.pEffect, nullptr);
  }

  //  �W���C�X�e�B�b�N�������I��
  pDirectInputData.pJoyStick->Acquire();
  JoyIDList_.push_back(DeviceNum_ - 1);
}
