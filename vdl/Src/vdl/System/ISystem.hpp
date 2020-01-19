#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Flag.hpp>

#include <vdl/Platform/Platform.hpp>
#include <vdl/ThreadPool/ThreadPool.hpp>

class ThreadPool;

class ISystem
{
public:
  static ISystem* Create();

  ISystem() = default;

  virtual ~ISystem() = default;

  [[nodiscard]] virtual PlatformFlags GetPlatform()const = 0;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual bool Update() = 0;

  virtual void EnableDefaultActions(const vdl::System::DefaultActionFlags& _Flags) = 0;

  virtual void DisableDefaultActions(const vdl::System::DefaultActionFlags& _Flags) = 0;

  virtual void ReportDefaultActions(const vdl::System::DefaultActionFlags& _Flags) = 0;

  virtual void SetMaxFPS(vdl::uint _MaxFPS) = 0;

  [[nodiscard]] virtual float GetDeltaTime()const = 0;

  [[nodiscard]] virtual vdl::uint GetFPS()const = 0;

  [[nodiscard]] virtual float GetCPUUseRate()const = 0;

  [[nodiscard]] virtual float GetMemoryUseRate()const = 0;

  [[nodiscard]] virtual ThreadPool* GetThreadPool() = 0;

  virtual void Pause() = 0;

  virtual void Resume() = 0;
};
