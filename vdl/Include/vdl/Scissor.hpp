#pragma once
#include "Type2.hpp"

namespace vdl
{
  struct Scissor
  {
    int2 LeftTop;
    int2 Size;
  public:
    Scissor() = default;

    Scissor(int _LeftTopX, int _LeftTopY, int _Width, int _Height)
      : LeftTop(_LeftTopX, _LeftTopY), Size(_Width, _Height) {}

    Scissor(const int2& _LeftTop, const int2& _Size)
      : LeftTop(_LeftTop), Size(_Size) {}
  };
}
