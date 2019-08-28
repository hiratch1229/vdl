#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Type2.hpp>
#include <vdl/Button.hpp>

class IGamepad
{
public:
  static IGamepad* Create();

  IGamepad() = default;

  virtual ~IGamepad() = default;

  virtual void Initialize() = 0;

  virtual void Update() = 0;

  virtual bool Press(vdl::uint _Number, vdl::uint _Index)const = 0;

  virtual bool Pressed(vdl::uint _Number, vdl::uint _Index)const = 0;

  virtual bool Released(vdl::uint _Number, vdl::uint _Index)const = 0;

  virtual bool Release(vdl::uint _Number, vdl::uint _Index)const = 0;

  virtual bool isConnect(vdl::uint _Index)const = 0;

  virtual int GetButtonNum(vdl::uint _Index)const = 0;

  virtual bool AnyButtonPress(vdl::uint _Index)const = 0;

  virtual bool AnyButtonPressed(vdl::uint _Index)const = 0;

  virtual bool AnyButtonReleased(vdl::uint _Index)const = 0;

  virtual vdl::input::Button GetButton(vdl::uint _Index, vdl::uint _Number)const = 0;

  virtual vdl::float2 GetLeftStick(vdl::uint _Index, float _DeadZone)const = 0;

  virtual vdl::float2 GetRightStick(vdl::uint _Index, float _DeadZone)const = 0;

  virtual float GetLeftTrigger(vdl::uint _Index, float _DeadZone)const = 0;

  virtual float GetRightTrigger(vdl::uint _Index, float _DeadZone)const = 0;

  virtual void SetVibration(vdl::uint _Index, float _Speed)const = 0;

  virtual void StopVibration(vdl::uint _Index)const = 0;
};
