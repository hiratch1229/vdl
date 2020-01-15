#include "IKeyboard.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CKeyboard.hpp"
#elif defined VDL_TARGET_LINUX
#include "Linux/CKeyboard.hpp"
#endif

IKeyboard* IKeyboard::Create()
{
  return new CKeyboard;
}
