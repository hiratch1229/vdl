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
    Image() = default;
  public:
    [[nodiscard]] uint Stride()const noexcept { return Size_.x * sizeof(Color); }

    [[nodiscard]] uint BufferSize()const noexcept { return static_cast<vdl::uint>(Buffer_.size()); }

    [[nodiscard]] uint2 GetSize()const noexcept { return Size_; }

    [[nodiscard]] bool isEmpty()const noexcept { return Buffer_.empty(); }

    [[nodiscard]] Color* Buffer() { return Buffer_.data(); }

    [[nodiscard]] const Color* Buffer()const { return Buffer_.data(); }

    void Resize(const uint2& _Size) { Size_ = _Size; Buffer_.resize(Size_.x * Size_.y); }
  public:
    CEREAL_SERIALIZE(Size_, Buffer_)
  };
}
