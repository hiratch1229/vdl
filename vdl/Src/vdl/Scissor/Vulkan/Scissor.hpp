#pragma once
#include <vdl/Scissor.hpp>

#include <vulkan/vulkan.hpp>

inline vk::Rect2D Cast(const vdl::Scissor& _Scissor)
{
  return { {_Scissor.LeftTop.x, _Scissor.LeftTop.y }, { _Scissor.Size.x, _Scissor.Size.y } };
}
