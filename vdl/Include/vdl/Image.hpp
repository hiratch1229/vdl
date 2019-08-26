#pragma once
#include "Type2.hpp"
#include "Color.hpp"
#include "Serialize.hpp"

#include <vector>

namespace vdl
{
  class Image
  {
    uint2 Size_ = 0;
    std::vector<Color> Buffer_;
  public:
    int Stride()const noexcept { return Size_.x * sizeof(Color); }
    uint BufferSize()const noexcept { return Buffer_.size(); }
    uint2 GetSize()const noexcept { return Size_; }
    Color* Buffer() { return Buffer_.data(); }
    void Resize(const uint2& _Size) { Size_ = _Size; Buffer_.resize(Size_.x * Size_.y); }
  public:
    Image() = default;
  public:
    CEREAL_SERIALIZE(Size_, Buffer_)
  };
}
