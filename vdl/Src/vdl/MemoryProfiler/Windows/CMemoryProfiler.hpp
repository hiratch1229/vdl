#pragma once
#include "../IMemoryProfiler.hpp"

#include <vdl/pch/Windows/pch.hpp>

class CMemoryProfiler : public IMemoryProfiler
{
  DWORDLONG MaxCapacity_;
public:
  CMemoryProfiler() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eWindows; }

  void Initialize()override;

  double GetUseRate()const override;
};
