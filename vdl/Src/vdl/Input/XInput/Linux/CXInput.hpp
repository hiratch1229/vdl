#pragma once
#include "../IXInput.hpp"

class CXInput : public IXInput
{
public:
  CXInput() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eLinux; }

  void Initialize()override {}

  void Update()override {}

  bool Press(vdl::uint _Index, vdl::uint _Code)const override { return false; }

  bool Pressed(vdl::uint _Index, vdl::uint _Code)const override { return false; }

  bool Released(vdl::uint _Index, vdl::uint _Code)const override { return  false; }

  bool Release(vdl::uint _Index, vdl::uint _Code)const override { return false; }

  bool isConnect(vdl::uint _Index)const override { return false; }

  vdl::float2 GetLeftStick(vdl::uint _Index, float _DeadZone)const override { return 0.0f; }

  vdl::float2 GetRightStick(vdl::uint _Index, float _DeadZone)const override { return 0.0f; }

  float GetLeftTrigger(vdl::uint _Index, float _DeadZone)const override { return 0.0f; }

  float GetRightTrigger(vdl::uint _Index, float _DeadZone)const override { return 0.0f; }

  void SetVibration(vdl::uint _Index, float _Speed)const override {}

  void StopVibration(vdl::uint _Index)const override {}
};
