#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Type2.hpp>

class IXInput
{
public:
  static IXInput* Create();

  IXInput() = default;

  virtual ~IXInput() = default;

  virtual void Initialize() = 0;

  virtual void Update() = 0;

  [[nodiscard]] virtual bool Press(vdl::uint _Index, vdl::uint _Code)const = 0;

  [[nodiscard]] virtual bool Pressed(vdl::uint _Index, vdl::uint _Code)const = 0;

  [[nodiscard]] virtual bool Released(vdl::uint _Index, vdl::uint _Code)const = 0;

  [[nodiscard]] virtual bool Release(vdl::uint _Index, vdl::uint _Code)const = 0;

  [[nodiscard]] virtual bool isConnect(vdl::uint _Index)const = 0;

  [[nodiscard]] virtual vdl::float2 GetLeftStick(vdl::uint _Index, float _DeadZone)const = 0;

  [[nodiscard]] virtual vdl::float2 GetRightStick(vdl::uint _Index, float _DeadZone)const = 0;

  [[nodiscard]] virtual float GetLeftTrigger(vdl::uint _Index, float _DeadZone)const = 0;

  [[nodiscard]] virtual float GetRightTrigger(vdl::uint _Index, float _DeadZone)const = 0;

  virtual void SetVibration(vdl::uint _Index, float _Speed)const = 0;

  virtual void StopVibration(vdl::uint _Index)const = 0;
};
