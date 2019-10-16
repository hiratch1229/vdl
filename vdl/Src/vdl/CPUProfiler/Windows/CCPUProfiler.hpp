#pragma once
#include "../ICPUProfiler.hpp"

#include <Pdh.h>

class CCPUProfiler : public ICPUProfiler
{
  HQUERY hQuery_;
  HCOUNTER hCounter_;
public:
  CCPUProfiler() = default;

  ~CCPUProfiler();

  void Initialize()override;

  double GetAsyncUseRate()const override;
};
