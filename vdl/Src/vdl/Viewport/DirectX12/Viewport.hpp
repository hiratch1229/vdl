#pragma once
#include <vdl/Viewport.hpp>

#include <d3d12.h>

inline D3D12_VIEWPORT Cast(const vdl::Viewport& _Viewport)
{
  return { _Viewport.LeftTop.x, _Viewport.LeftTop.y, _Viewport.Size.x, _Viewport.Size.y, _Viewport.MinDepth, _Viewport.MaxDepth };
}
