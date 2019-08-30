#include "IXInput.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CXInput.hpp"
#endif

IXInput* IXInput::Create()
{
  return new CXInput;
}
