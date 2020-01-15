#pragma once
#include "../IMouse.hpp"

class CMouse : public IMouse
{
public:
  CMouse() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eLinux; }

  void Initialize()override {}

  void Update()override {}

  bool Press(vdl::uint _Code)const override { return false; }

  bool Pressed(vdl::uint _Code)const override { return  false; }

  bool Released(vdl::uint _Code)const override { return false; }

  bool Release(vdl::uint _Code)const override { return  false; }

  vdl::int2 GetPos()const override { return 0.0f; }

  vdl::int2 GetDelta()const override { return 0.0f; }

  vdl::int2 GetWheel()const override { return 0.0f; }

  void SetPos(const vdl::int2& _Pos)override {}

  void Scroll(const vdl::int2& _Scroll)override {}
};
