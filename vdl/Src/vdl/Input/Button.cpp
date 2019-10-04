#include <vdl/Button.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Input/Keyboard/IKeyboard.hpp>
#include <vdl/Input/Mouse/IMouse.hpp>
#include <vdl/Input/XInput/IXInput.hpp>
#include <vdl/Input/Gamepad/IGamepad.hpp>

#include <assert.h>

namespace vdl::Input
{
  bool isPress(const Button& _Button)
  {
    switch (_Button.GetInputDevice())
    {
    case InputDeviceType::eKeyboard:
      return Engine::Get<IKeyboard>()->Press(_Button.GetCode());
    case InputDeviceType::eMouse:
      return Engine::Get<IMouse>()->Press(_Button.GetCode());
    case InputDeviceType::eXInput:
      return Engine::Get<IXInput>()->Press(_Button.GetIndex(), _Button.GetCode());
    case InputDeviceType::eGamepad:
      return Engine::Get<IGamepad>()->Press(_Button.GetIndex(), _Button.GetCode());
    default: assert(false);
    }

    return false;
  }

  bool isPressed(const Button& _Button)
  {
    switch (_Button.GetInputDevice())
    {
    case InputDeviceType::eKeyboard:
      return Engine::Get<IKeyboard>()->Pressed(_Button.GetCode());
    case InputDeviceType::eMouse:
      return Engine::Get<IMouse>()->Pressed(_Button.GetCode());
    case InputDeviceType::eXInput:
      return Engine::Get<IXInput>()->Pressed(_Button.GetIndex(), _Button.GetCode());
    case InputDeviceType::eGamepad:
      return Engine::Get<IGamepad>()->Pressed(_Button.GetIndex(), _Button.GetCode());
    default: assert(false);
    }

    return false;
  }

  bool isReleased(const Button& _Button)
  {
    switch (_Button.GetInputDevice())
    {
    case InputDeviceType::eKeyboard:
      return Engine::Get<IKeyboard>()->Released(_Button.GetCode());
    case InputDeviceType::eMouse:
      return Engine::Get<IMouse>()->Released(_Button.GetCode());
    case InputDeviceType::eXInput:
      return Engine::Get<IXInput>()->Released(_Button.GetIndex(), _Button.GetCode());
    case InputDeviceType::eGamepad:
      return Engine::Get<IGamepad>()->Released(_Button.GetIndex(), _Button.GetCode());
    default: assert(false);
    }

    return false;
  }

  bool isRelease(const Button& _Button)
  {
    switch (_Button.GetInputDevice())
    {
    case InputDeviceType::eKeyboard:
      return Engine::Get<IKeyboard>()->Release(_Button.GetCode());
    case InputDeviceType::eMouse:
      return Engine::Get<IMouse>()->Release(_Button.GetCode());
    case InputDeviceType::eXInput:
      return Engine::Get<IXInput>()->Release(_Button.GetIndex(), _Button.GetCode());
    case InputDeviceType::eGamepad:
      return Engine::Get<IGamepad>()->Release(_Button.GetIndex(), _Button.GetCode());
    default: assert(false);
    }

    return false;
  }
}
