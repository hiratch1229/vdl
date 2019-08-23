#pragma once
#include "Fwd.hpp"


namespace vdl::input
{
  //  入力デバイス
  enum class InputDevice
  {
    None,
    Keyboard,
    Mouse,
    XInput,
    Gamepad,
  };
}
