#pragma once
#include <vdl/Platform.hpp>

#if defined VDL_TARGET_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined VDL_TARGET_LINUX

#endif

#include <vulkan/vulkan.hpp>
