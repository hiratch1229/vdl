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

  float GetDeltaTime()
  {
    return Engine::Get<ISystem>()->GetDeltaTime();
  }
}
