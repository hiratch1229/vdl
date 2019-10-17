#include <vdl/System.hpp>

#include <vdl/Engine.hpp>
#include <vdl/System/ISystem.hpp>

namespace vdl::System
{
  bool Update()
  {
    return Engine::Get<ISystem>()->Update();
  }

  void Exit()
  {
    Engine::Get<ISystem>()->EnableDefaultActions(DefalutAction::eExit);
    Engine::Get<ISystem>()->ReportDefaultActions(DefalutAction::eExit);
    Engine::Get<ISystem>()->DisableDefaultActions(DefalutAction::eExit);
  }

  void EnableDefaultActions(uint _DefaultActionFlags)
  {
    Engine::Get<ISystem>()->EnableDefaultActions(_DefaultActionFlags);
  }

  void DisableDefaultActions(uint _DefaultActionFlags)
  {
    Engine::Get<ISystem>()->DisableDefaultActions(_DefaultActionFlags);
  }

  void SetMaxFPS(uint _MaxFPS)
  {
    Engine::Get<ISystem>()->SetMaxFPS(_MaxFPS);
  }

  float GetDeltaTime()
  {
    return Engine::Get<ISystem>()->GetDeltaTime();
  }

  uint GetFPS()
  {
    return Engine::Get<ISystem>()->GetFPS();
  }

  float GetCPUUseRate()
  {
    return Engine::Get<ISystem>()->GetCPUUseRate();
  }

  float GetMemoryUseRate()
  {
    return Engine::Get<ISystem>()->GetMemoryUseRate();
  }
}
