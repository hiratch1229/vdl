#pragma once
#include "ISystem.hpp"

#include <chrono>

class ISwapChain;
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
  ISwapChain* pSwapChain_;
  ICPUProfiler* pCPUProfiler_;
  IMemoryProfiler* pMemoryProfiler_;
  IKeyboard* pKeyboard_;
  IMouse* pMouse_;
  IXInput* pXInput_;
  IGamepad* pGamepad_;
  IRenderer* pRenderer_;
  IGUI* pGUI_;
private:
private:
  SystemState SystemState_;
  vdl::uint DefaultActionFlags_ = 0;
  vdl::uint ValidDefaultActions_ = ~0;
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

  void Initialize()override;

  bool Update()override;

  void EnableDefaultActions(vdl::uint _DefaultActionFlags)override { ValidDefaultActions_ |= _DefaultActionFlags; }

  void DisableDefaultActions(vdl::uint _DefaultActionFlags)override { ValidDefaultActions_ &= ~_DefaultActionFlags; }

  void ReportDefaultActions(vdl::uint _DefaultActionFlags)override { DefaultActionFlags_ |= (ValidDefaultActions_ & _DefaultActionFlags); }

  void SetMaxFPS(vdl::uint _MaxFPS)override { FrameInterval_ = Duration(_MaxFPS == 0 ? 0.0 : 1.0 / _MaxFPS); }

  float GetDeltaTime()const override { return static_cast<float>(DeltaTime_.count()); }

  vdl::uint GetFPS()const override { return CurrentFPS_; }

  float GetCPUUseRate()const override { return static_cast<float>(CurrentCPUUseRate_); }

  float GetMemoryUseRate()const override { return static_cast<float>(CurrentMemoryUseRate_); }

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
