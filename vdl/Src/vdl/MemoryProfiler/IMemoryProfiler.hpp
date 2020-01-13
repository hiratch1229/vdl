#pragma once
#include <vdl/Platform/Platform.hpp>

class IMemoryProfiler
{
public:
  static IMemoryProfiler* Create();

  IMemoryProfiler() = default;

  virtual ~IMemoryProfiler() = default;

  [[nodiscard]] virtual PlatformFlags GetPlatform()const = 0;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual double GetUseRate()const = 0;
};
