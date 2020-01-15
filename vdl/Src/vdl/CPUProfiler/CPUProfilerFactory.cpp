#include "ICPUProfiler.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CCPUProfiler.hpp"
#elif defined VDL_TARGET_LINUX
#include "Linux/CCPUProfiler.hpp"
#endif

ICPUProfiler* ICPUProfiler::Create()
{
  return new CCPUProfiler;
}
