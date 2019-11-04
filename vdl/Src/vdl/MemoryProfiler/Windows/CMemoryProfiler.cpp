#include "CMemoryProfiler.hpp"

#include <vdl/DetectMemoryLeak.hpp>

#include <sysinfoapi.h>
#include <psapi.h>

void CMemoryProfiler::Initialize()
{
  MEMORYSTATUSEX MemoryStatus = { sizeof(MEMORYSTATUSEX) };
  ::GlobalMemoryStatusEx(&MemoryStatus);

  MaxCapacity_ = MemoryStatus.ullTotalPhys;
}

double CMemoryProfiler::GetUseRate()const
{
  PROCESS_MEMORY_COUNTERS MemoryCounter;
  if (!::GetProcessMemoryInfo(::GetCurrentProcess(), &MemoryCounter, sizeof(MemoryCounter)))
  {
    return 0.0;
  }

  return (MemoryCounter.WorkingSetSize / static_cast<double>(MaxCapacity_)) * 100.0;
}
