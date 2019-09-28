#include <vdl/Mouse.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Input/Mouse/IMouse.hpp>

namespace vdl::Input::Mouse
{
  int2 GetPos()
  {
    return Engine::Get<IMouse>()->GetPos();
  }

  void SetPos(const int2& _Pos)
  {
    Engine::Get<IMouse>()->SetPos(_Pos);
  }

  int2 GetDelta()
  {
    return Engine::Get<IMouse>()->GetDelta();
  }

  int2 GetWheel()
  {
    return Engine::Get<IMouse>()->GetWheel();
  }
}
