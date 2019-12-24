#include <vdl/System.hpp>

#include <vdl/Engine.hpp>
#include <vdl/System/ISystem.hpp>

#include <vdl/Flag.hpp>

namespace vdl::System
{
  bool Update()
  {
    return Engine::Get<ISystem>()->Update();
  }

  void Exit()
  {
    constexpr DefaultActionFlags kActionFlag = DefaultActionFlag::eExit;

    ISystem* pSystem = Engine::Get<ISystem>();

    pSystem->EnableDefaultActions(kActionFlag);
    pSystem->ReportDefaultActions(kActionFlag);
    pSystem->DisableDefaultActions(kActionFlag);
  }

  void EnableDefaultActions(const DefaultActionFlags& _DefaultActionFlags)
  {
    Engine::Get<ISystem>()->EnableDefaultActions(_DefaultActionFlags);
  }

  void DisableDefaultActions(const DefaultActionFlags& _DefaultActionFlags)
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
