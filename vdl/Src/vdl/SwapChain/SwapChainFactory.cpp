#include "ISwapChain.hpp"

#if defined VDL_EXECUTE_DIRECTX11
#include "DirectX11/CSwapChain.hpp"
#elif defined VDL_EXECUTE_DIRECTX12
#include "DirectX12/CSwapChain.hpp"
#elif defined VDL_EXECUTE_VULKAN
#include "Vulkan/CSwapChain.hpp"
#endif

ISwapChain* ISwapChain::Create()
{
  return new CSwapChain;
}
