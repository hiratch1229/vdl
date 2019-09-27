#include <vdl/Image.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <ThirdParty/stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ThirdParty/stb/stb_image_write.h>

namespace vdl
{
  Image::Image(const CompressionImage& _CompressionImage)
  {
    int2 TextureSize;
    int Comp;
    uint8_t* Data = ::stbi_load_from_memory(_CompressionImage.Buffer(), _CompressionImage.BufferSize(), &TextureSize.x, &TextureSize.y, &Comp, 0);
    assert(Data);

    Resize(TextureSize);
    ::memcpy(Buffer_.data(), Data, BufferSize());

    STBIW_FREE(Data);
  }

  CompressionImage::CompressionImage(const Image& _Image)
    : Size_(_Image.GetSize())
  {
    int Lenght;
    uint8_t* PNG = ::stbi_write_png_to_mem(&_Image.Buffer()->Red, static_cast<int>(Size_.x * 4), static_cast<int>(Size_.x), static_cast<int>(Size_.y), sizeof(Color), &Lenght);
    assert(PNG);

    Buffer_.resize(static_cast<int>(Lenght));
    ::memcpy(Buffer_.data(), PNG, Lenght);

    STBIW_FREE(PNG);
  }
}