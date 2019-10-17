#pragma once

class ICPUProfiler
{
public:
  static ICPUProfiler* Create();

  ICPUProfiler() = default;

  virtual ~ICPUProfiler() = default;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual double GetUseRate()const = 0;
};
