#pragma once
#include "../IMemoryProfiler.hpp"

#include <Windows.h>

class CMemoryProfiler : public IMemoryProfiler
{
  DWORDLONG MaxCapacity_;
public:
  CMemoryProfiler() = default;

  virtual ~CMemoryProfiler() = default;

  void Initialize()override;

  double GetUseRate()const override;
};
