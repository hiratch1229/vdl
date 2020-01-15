#include "IGamepad.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CGamepad.hpp"
#elif defined VDL_TARGET_LINUX
#include "Linux/CGamepad.hpp"
#endif

IGamepad* IGamepad::Create()
{
  return new CGamepad;
}
