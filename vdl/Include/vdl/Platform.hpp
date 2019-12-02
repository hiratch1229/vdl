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

#if defined VDL_EXECUTE_DIRECTX11
#ifdef _DEBUG
#pragma comment(lib, "vdlForDirectX11OnWindows_d.lib")
#else
#pragma comment(lib, "vdlForDirectX11OnWindows.lib")
#endif // _DEBUG
#elif defined VDL_EXECUTE_DIRECTX12
#ifdef _DEBUG
#pragma comment(lib, "vdlForDirectX12OnWindows_d.lib")
#else
#pragma comment(lib, "vdlForDirectX12OnWindows.lib")
#endif // _DEBUG
#elif defined VDL_EXECUTE_VULKAN
#ifdef _DEBUG
#pragma comment(lib, "vdlForVulkanOnWindows_d.lib")
#else
#pragma comment(lib, "vdlForVulkanOnWindows.lib")
#endif // _DEBUG
#endif
