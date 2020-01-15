#pragma once
#include "../IKeyboard.hpp"

class CKeyboard : public IKeyboard
{
public:
  CKeyboard() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eLinux; }

  void Initialize()override {}

  void Update()override {}

  bool Press(vdl::uint _Code)const override
  {
    return false;
  }

  bool Pressed(vdl::uint _Code)const override
  {
    return false;
  }

  bool Released(vdl::uint _Code)const override
  {
    return false;
  }

  bool Release(vdl::uint _Code)const override
  {
    return false;
  }
};
