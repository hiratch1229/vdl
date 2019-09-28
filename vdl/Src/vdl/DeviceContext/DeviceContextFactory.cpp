#include "IDeviceContext.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_EXECUTE_DIRECTX11
#include "DirectX11/CDeviceContext.hpp"
#elif defined VDL_EXECUTE_DIRECTX12
#include "DirectX12/CDeviceContext.hpp"
#elif defined VDL_EXECUTE_VULKAN
#include "Vulkan/CDeviceContext.hpp"
#endif

IDeviceContext* IDeviceContext::Create()
{
  return new CDeviceContext;
}
