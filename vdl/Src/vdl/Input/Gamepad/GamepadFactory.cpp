#include "IGamepad.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CGamepad.hpp"
#endif

IGamepad* IGamepad::Create()
{
  return new CGamepad;
}
