#pragma once
#include "Type2.hpp"

namespace vdl
{
  struct Scissor
  {
    uint2 LeftTop;
    uint2 Size;
  public:
    Scissor() = default;

    Scissor(uint _LeftTopX, uint _LeftTopY, uint _Width, uint _Height)
      : LeftTop(_LeftTopX, _LeftTopY), Size(_Width, _Height) {}

    Scissor(const uint2& _LeftTop, const uint2& _Size)
      : LeftTop(_LeftTop), Size(_Size) {}
  };
}
