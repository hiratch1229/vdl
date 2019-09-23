#include "IBufferManager.hpp"

#include <vdl/Platform.hpp>

#if defined VDL_EXECUTE_DIRECTX11
#include "DirectX11/CBufferManager.hpp"
#elif defined VDL_EXECUTE_DIRECTX12
#include "DirectX12/CBufferManager.hpp"
#elif defined VDL_EXECUTE_VULKAN
#include "Vulkan/CBufferManager.hpp"
#endif

IBufferManager* IBufferManager::Create()
{
  return new CBufferManager;
}
