#include "ISwapChain.hpp"

#if defined VDL_EXECUTE_DIRECTX11

#elif defined VDL_EXECUTE_DIRECTX12

#elif defined VDL_EXECUTE_VULKAN

#endif

ISwapChain* ISwapChain::Create()
{
  return new CSwapChain;
}
