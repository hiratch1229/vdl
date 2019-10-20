#pragma once
#include "Fwd.hpp"
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

    Image(const CompressionImage& _CompressionImage);
  public:
    [[nodiscard]] uint Stride()const noexcept { return Size_.x * 4; }

    [[nodiscard]] uint BufferSize()const noexcept { return static_cast<vdl::uint>(Buffer_.size() * 4); }

    [[nodiscard]] uint2 GetSize()const noexcept { return Size_; }

    [[nodiscard]] bool isEmpty()const noexcept { return Buffer_.empty(); }

    [[nodiscard]] Color* Buffer() { return Buffer_.data(); }

    [[nodiscard]] const Color* Buffer()const { return Buffer_.data(); }

    void Resize(const uint2& _Size) { Size_ = _Size; Buffer_.resize(static_cast<size_t>(Size_.x) * Size_.y); }
  public:
    [[nodiscard]] std::vector<uint8_t> EncodePNG()const;

    bool SavePNG(const char* _Directory)const;
  };

  class CompressionImage
  {
    uint2 Size_ = 0;
    std::vector<uint8_t> Buffer_;
  public:
    CompressionImage() = default;

    CompressionImage(const Image& _Image);
  public:
    [[nodiscard]] uint BufferSize()const noexcept { return static_cast<vdl::uint>(Buffer_.size()); }

    [[nodiscard]] uint2 GetSize()const noexcept { return Size_; }

    [[nodiscard]] bool isEmpty()const noexcept { return Buffer_.empty(); }

    [[nodiscard]] const uint8_t* Buffer()const { return Buffer_.data(); }
  public:
    CEREAL_SERIALIZE(Size_, Buffer_)
  };
}
