#include "IMouse.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CMouse.hpp"
#elif defined VDL_TARGET_LINUX
#include "Linux/CMouse.hpp"
#endif

IMouse* IMouse::Create()
{
  return new CMouse;
}
