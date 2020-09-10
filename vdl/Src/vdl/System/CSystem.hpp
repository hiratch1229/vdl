#pragma once
#include "ISystem.hpp"

#include <vdl/System.hpp>

#include <chrono>

class IWindow;
class IDeviceContext;
class ICPUProfiler;
class IMemoryProfiler;
class IKeyboard;
class IMouse;
class IXInput;
class IGamepad;
class IRenderer;
class IGUI;

class CSystem : public ISystem
{
  enum class SystemState
  {
    eInitialized,
    eRunning,
    ePause,
  };
private:
  IWindow* pWindow_;
  IDeviceContext* pDeviceContext_;
  ICPUProfiler* pCPUProfiler_;
  IMemoryProfiler* pMemoryProfiler_;
  IKeyboard* pKeyboard_;
  IMouse* pMouse_;
  IXInput* pXInput_;
  IGamepad* pGamepad_;
  IRenderer* pRenderer_;
  IGUI* pGUI_;
private:
  ThreadPool ThreadPool_;
  SystemState SystemState_;
  vdl::System::DefaultActionFlags DefaultActionFlags_ = 0;
  vdl::System::DefaultActionFlags ValidDefaultActions_ = ~0;
  using Duration = std::chrono::duration<double>;
  std::chrono::high_resolution_clock::time_point LastTime_;
  Duration DeltaTime_;
  Duration FrameInterval_;
  Duration TimeStamp_;
  Duration TimeTlapsed_;
  vdl::uint Frames_ = 0;
  vdl::uint CurrentFPS_ = 0;
  double CurrentCPUUseRate_;
  double CurrentMemoryUseRate_;
public:
  CSystem() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eCommon; }

  void Initialize()override;

  bool Update()override;

  void EnableDefaultActions(const vdl::System::DefaultActionFlags& _Flags)override { ValidDefaultActions_ |= _Flags; }

  void DisableDefaultActions(const vdl::System::DefaultActionFlags& _Flags)override { ValidDefaultActions_ &= ~_Flags; }

  void ReportDefaultActions(const vdl::System::DefaultActionFlags& _Flags)override { DefaultActionFlags_ |= (ValidDefaultActions_ & _Flags); }

  void SetMaxFPS(vdl::uint _MaxFPS)override { FrameInterval_ = Duration(_MaxFPS == 0 ? 0.0 : 1.0 / _MaxFPS); }

  float GetDeltaTime()const override { return static_cast<float>(DeltaTime_.count()); }

  vdl::uint GetFPS()const override { return CurrentFPS_; }

  float GetCPUUseRate()const override { return static_cast<float>(CurrentCPUUseRate_); }

  float GetMemoryUseRate()const override { return static_cast<float>(CurrentMemoryUseRate_); }

  ThreadPool* GetThreadPool()override { return &ThreadPool_; }

  void Pause()override { SystemState_ = SystemState::ePause; }

  void Resume()override
  {
    if (SystemState_ != SystemState::ePause)
    {
      return;
    }

    SystemState_ = SystemState::eRunning;
    LastTime_ = std::chrono::high_resolution_clock::now();
  }
};
