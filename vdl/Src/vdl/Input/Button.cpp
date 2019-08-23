#include <vdl/Button.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Input/Keyboard/IKeyboard.hpp>
#include <vdl/Input/Mouse/IMouse.hpp>
#include <vdl/Input/XInput/IXInput.hpp>
#include <vdl/Input/Gamepad/IGamepad.hpp>

#include <assert.h>

namespace vdl::input
{
  bool Press(const Button& _Button)
  {
    switch (_Button.GetInputDevice())
    {
    case InputDevice::eKeyboard:
      return Engine::Get<IKeyboard>()->Press(_Button.GetCode());
    case InputDevice::eMouse:
      return Engine::Get<IMouse>()->Press(_Button.GetCode());
    case InputDevice::eXInput:
      return Engine::Get<IXInput>()->Press(_Button.GetCode(), _Button.GetIndex());
    case InputDevice::eGamepad:
      return Engine::Get<IGamepad>()->Press(_Button.GetCode(), _Button.GetIndex());
    default: assert(false);
    }

    return false;
  }

  bool Pressed(const Button& _Button)
  {
    switch (_Button.GetInputDevice())
    {
    case InputDevice::eKeyboard:
      return Engine::Get<IKeyboard>()->Pressed(_Button.GetCode());
    case InputDevice::eMouse:
      return Engine::Get<IMouse>()->Pressed(_Button.GetCode());
    case InputDevice::eXInput:
      return Engine::Get<IXInput>()->Pressed(_Button.GetCode(), _Button.GetIndex());
    case InputDevice::eGamepad:
      return Engine::Get<IGamepad>()->Pressed(_Button.GetCode(), _Button.GetIndex());
    default: assert(false);
    }

    return false;
  }

  bool Released(const Button& _Button)
  {
    switch (_Button.GetInputDevice())
    {
    case InputDevice::eKeyboard:
      return Engine::Get<IKeyboard>()->Released(_Button.GetCode());
    case InputDevice::eMouse:
      return Engine::Get<IMouse>()->Released(_Button.GetCode());
    case InputDevice::eXInput:
      return Engine::Get<IXInput>()->Released(_Button.GetCode(), _Button.GetIndex());
    case InputDevice::eGamepad:
      return Engine::Get<IGamepad>()->Released(_Button.GetCode(), _Button.GetIndex());
    default: assert(false);
    }

    return false;
  }

  bool Release(const Button& _Button)
  {
    switch (_Button.GetInputDevice())
    {
    case InputDevice::eKeyboard:
      return Engine::Get<IKeyboard>()->Release(_Button.GetCode());
    case InputDevice::eMouse:
      return Engine::Get<IMouse>()->Release(_Button.GetCode());
    case InputDevice::eXInput:
      return Engine::Get<IXInput>()->Release(_Button.GetCode(), _Button.GetIndex());
    case InputDevice::eGamepad:
      return Engine::Get<IGamepad>()->Release(_Button.GetCode(), _Button.GetIndex());
    default: assert(false);
    }

    return false;
  }
}
