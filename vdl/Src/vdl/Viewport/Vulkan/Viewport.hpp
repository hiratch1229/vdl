#pragma once
#include <vdl/Viewport.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

inline vk::Viewport Cast(const vdl::Viewport& _Viewport)
{
  return { _Viewport.LeftTop.x, _Viewport.LeftTop.y + _Viewport.Size.y, _Viewport.Size.x, -_Viewport.Size.y, _Viewport.MinDepth, _Viewport.MaxDepth };
}
