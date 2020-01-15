#pragma once
#include "../IMemoryProfiler.hpp"

class CMemoryProfiler : public IMemoryProfiler
{
public:
  CMemoryProfiler() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eLinux; }

  void Initialize()override{}

  double GetUseRate()const override { return 0.0; }
};
