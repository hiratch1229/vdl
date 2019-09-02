#include "IDevice.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_EXECUTE_DIRECTX11

#elif defined VDL_EXECUTE_DIRECTX12

#elif defined VDL_EXECUTE_VULKAN

#endif

IDevice* IDevice::Create()
{
  return new CDevice;
}
