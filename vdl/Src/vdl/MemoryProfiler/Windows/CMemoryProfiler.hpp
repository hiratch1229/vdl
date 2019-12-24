#pragma once
#include "../IMemoryProfiler.hpp"

#include <vdl/pch/Windows/pch.hpp>

class CMemoryProfiler : public IMemoryProfiler
{
  DWORDLONG MaxCapacity_;
public:
  CMemoryProfiler() = default;

  PlatformType GetPlatform()const final { return PlatformType::eWindows; }

  void Initialize()override;

  double GetUseRate()const override;
};
