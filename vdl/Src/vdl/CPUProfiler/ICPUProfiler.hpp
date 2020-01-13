#pragma once
#include <vdl/Platform/Platform.hpp>

class ICPUProfiler
{
public:
  static ICPUProfiler* Create();

  ICPUProfiler() = default;

  virtual ~ICPUProfiler() = default;

  [[nodiscard]] virtual PlatformFlags GetPlatform()const = 0;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual double GetUseRate()const = 0;
};
