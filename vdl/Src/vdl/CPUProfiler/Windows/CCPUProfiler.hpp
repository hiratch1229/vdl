#pragma once
#include "../ICPUProfiler.hpp"

#include <vdl/Fwd.hpp>

#include <Pdh.h>

class CCPUProfiler : public ICPUProfiler
{
  HQUERY hQuery_;
  HCOUNTER hCounter_;
  vdl::uint CoreNum_;
public:
  CCPUProfiler() = default;

  ~CCPUProfiler();

  PlatformType GetPlatform()const final { return PlatformType::eWindows; }

  void Initialize()override;

  double GetUseRate()const override;
};
