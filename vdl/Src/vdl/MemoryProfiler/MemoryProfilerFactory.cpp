#include "IMemoryProfiler.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#include "Windows/CMemoryProfiler.hpp"
#elif defined VDL_TARGET_LINUX
#include "Linux/CMemoryProfiler.hpp"
#endif

IMemoryProfiler* IMemoryProfiler::Create()
{
  return new CMemoryProfiler;
}
