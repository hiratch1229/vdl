#pragma once
#include "../ICPUProfiler.hpp"

class CCPUProfiler : public ICPUProfiler
{
public:
  CCPUProfiler() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eLinux; }

  void Initialize()override {}

  double GetUseRate()const override { return 0.0; }
};
