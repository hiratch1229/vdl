#include "IMouse.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CMouse.hpp"
#endif

IMouse* IMouse::Create()
{
  return new CMouse;
}
