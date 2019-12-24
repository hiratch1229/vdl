#pragma once
#include "Fwd.hpp"

namespace vdl::System
{
  enum class DefaultActionFlag : uint8_t
  {
    eNone = 0,

    eExit = 1 << 0,

    eChangeWindowMode = 1 << 1,

    eScreenShot = 1 << 2,

    eAll = static_cast<uint8_t>(~0),
  };

  [[nodiscard]] bool Update();

  void Exit();

  void EnableDefaultActions(const DefaultActionFlags& _DefaultActionFlags);

  void DisableDefaultActions(const DefaultActionFlags& _DefaultActionFlags);

  void SetMaxFPS(uint _MaxFPS);

  [[nodiscard]] float GetDeltaTime();

  [[nodiscard]] uint GetFPS();

  [[nodiscard]] float GetCPUUseRate();

  [[nodiscard]] float GetMemoryUseRate();
}
