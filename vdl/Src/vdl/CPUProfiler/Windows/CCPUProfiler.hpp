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

  PlatformFlags GetPlatform()const final { return PlatformFlag::eWindows; }

  void Initialize()override;

  double GetUseRate()const override;
};
