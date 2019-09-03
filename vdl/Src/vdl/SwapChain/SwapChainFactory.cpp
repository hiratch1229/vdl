#include "ISwapChain.hpp"

#if defined VDL_EXECUTE_DIRECTX11

#elif defined VDL_EXECUTE_DIRECTX12
#include "DirectX12/CSwapChain.hpp"
#elif defined VDL_EXECUTE_VULKAN

#endif

ISwapChain* ISwapChain::Create()
{
  return new CSwapChain;
}
