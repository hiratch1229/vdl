#pragma once
#include "ISystem.hpp"

#include <Windows.h>

class ISwapChain;
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
  IKeyboard* pKeyboard_;
  IMouse* pMouse_;
  IXInput* pXInput_;
  IGamepad* pGamepad_;
  IRenderer* pRenderer_;
  IGUI* pGUI_;
private:
  SystemState SystemState_;
  vdl::uint DefaultActionFlags_ = 0;
  vdl::uint ValidDefaultActions_ = ~0;
  double MilliSecondsPerFrequency_ = 0.0f;
  double FrameInterval_ = 0.0;
  double LastTime_ = 0.0;
  double DeltaTime_ = 0.0;
private:
  double GetMicroSecond()const;
public:
  CSystem() = default;

  void Initialize()override;

  bool Update()override;

  void EnableDefaultActions(vdl::uint _DefaultActionFlags)override { ValidDefaultActions_ |= _DefaultActionFlags; }

  void DisableDefaultActions(vdl::uint _DefaultActionFlags)override { ValidDefaultActions_ &= ~_DefaultActionFlags; }

  void ReportDefaultActions(vdl::uint _DefaultActionFlags)override { DefaultActionFlags_ |= _DefaultActionFlags; }

  void SetMaxFramerate(vdl::uint _MaxFramerate)override { FrameInterval_ = (_MaxFramerate == 0 ? 0.0 : 1.0 / _MaxFramerate) * MilliSecondsPerFrequency_; }

  float GetDeltaTime()override { return static_cast<float>(DeltaTime_); }

  void Pause()override { SystemState_ = SystemState::ePause; }

  void Resume()override
  {
    if (SystemState_ != SystemState::ePause)
    {
      return;
    }

    SystemState_ = SystemState::eRunning;
    LastTime_ = GetMicroSecond();
  }
};
