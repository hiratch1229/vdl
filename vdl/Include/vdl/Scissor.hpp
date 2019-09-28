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

    constexpr Scissor(uint _LeftTopX, uint _LeftTopY, uint _Width, uint _Height)
      : LeftTop(_LeftTopX, _LeftTopY), Size(_Width, _Height) {}

    constexpr Scissor(const uint2& _LeftTop, const uint2& _Size)
      : LeftTop(_LeftTop), Size(_Size) {}
  public:
    [[nodiscard]] constexpr bool operator==(const Scissor& _Scissor)const { return LeftTop == _Scissor.LeftTop && Size == _Scissor.Size; }

    [[nodiscard]] constexpr bool operator!=(const Scissor& _Scissor)const { return LeftTop != _Scissor.LeftTop || Size != _Scissor.Size; }
  };
}
