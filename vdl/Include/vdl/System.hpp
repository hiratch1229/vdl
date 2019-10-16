#pragma once
#include "Fwd.hpp"

namespace vdl::System
{
  namespace DefalutAction
  {
    enum Flags : uint
    {
      eNone = 0,

      eExit = 1 << 0,

      eChangeWindowMode = 1 << 1,

      eScreenShot = 1 << 2,

      eAll = ~0u,
    };
  }

  [[nodiscard]] bool Update();

  void Exit();

  void EnableDefaultActions(uint _DefaultActionFlags);

  void DisableDefaultActions(uint _DefaultActionFlags);

  void SetMaxFPS(uint _MaxFPS);

  [[nodiscard]] float GetDeltaTime();

  [[nodiscard]] uint GetFPS();

  [[nodiscard]] float GetCPUUseRate();
}
