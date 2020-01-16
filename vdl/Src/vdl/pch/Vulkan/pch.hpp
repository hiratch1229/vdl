#pragma once
#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined VDL_TARGET_LINUX
#define VK_USE_PLATFORM_XCB_KHR
#endif

#include <vulkan/vulkan.hpp>
