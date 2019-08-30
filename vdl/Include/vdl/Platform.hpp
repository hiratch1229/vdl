#pragma once

#if defined VDL_USING_WINDOWS
#define VDL_TARGET_WINDOWS
#endif

#if defined VDL_USING_DIRECTX11
#define VDL_EXECUTE_DIRECTX11
#elif defined VDL_USING_DIRECTX12
#define VDL_EXECUTE_DIRECTX12
#elif defined VDL_USING_VULKAN
#define VDL_EXECUTE_VULKAN
#endif
