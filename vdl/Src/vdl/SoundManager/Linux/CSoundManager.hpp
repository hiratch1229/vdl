#pragma once
#include "../ISoundManager.hpp"

class CSoundManager : public ISoundManager
{
public:
  CSoundManager() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eLinux; }

  void Initialize()override {}

  vdl::ID Load(const char* _FilePath)override { return 0; }

  void AddRef(vdl::ID _ID)override {}

  void Release(vdl::ID _ID)override {}
};
