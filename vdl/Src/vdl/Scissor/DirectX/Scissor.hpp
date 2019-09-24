#pragma once
#include <vdl/Scissor.hpp>

#include <windef.h>

inline RECT Cast(const vdl::Scissor& _Scissor)
{
  return { static_cast<LONG>(_Scissor.LeftTop.x), static_cast<LONG>(_Scissor.LeftTop.y),
  static_cast<LONG>(_Scissor.LeftTop.x + _Scissor.Size.x), static_cast<LONG>(_Scissor.LeftTop.y + _Scissor.Size.y) };
}
