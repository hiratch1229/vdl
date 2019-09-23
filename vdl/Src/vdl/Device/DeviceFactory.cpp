#include "IDevice.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_EXECUTE_DIRECTX11
#include "DirectX11/CDevice.hpp"
#elif defined VDL_EXECUTE_DIRECTX12
#include "DirectX12/CDevice.hpp"
#elif defined VDL_EXECUTE_VULKAN
#include "Vulkan/CDevice.hpp"
#endif

IDevice* IDevice::Create()
{
  return new CDevice;
}
