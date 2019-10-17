#pragma once

class IMemoryProfiler
{
public:
  static IMemoryProfiler* Create();

  IMemoryProfiler() = default;

  virtual ~IMemoryProfiler() = default;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual double GetUseRate()const = 0;
};
