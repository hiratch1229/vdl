#include "IXInput.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CXInput.hpp"
#elif defined VDL_TARGET_LINUX
#include "Linux/CXInput.hpp"
#endif

IXInput* IXInput::Create()
{
  return new CXInput;
}
