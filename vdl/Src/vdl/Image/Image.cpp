#include <vdl/Image.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <ThirdParty/stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ThirdParty/stb/stb_image_write.h>

#include <string>
#include <filesystem>
#include <fstream>

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

  std::vector<uint8_t> Image::EncodePNG()const
  {
    assert(!isEmpty());

    std::vector<uint8_t> EncodeData;
    {
      int Lenght;
      uint8_t* PNG = ::stbi_write_png_to_mem(&Buffer()->Red, static_cast<int>(Size_.x * 4), static_cast<int>(Size_.x), static_cast<int>(Size_.y), sizeof(Color), &Lenght);

      EncodeData.resize(static_cast<size_t>(Lenght));
      ::memcpy(EncodeData.data(), PNG, Lenght);

      STBIW_FREE(PNG);
    }

    return EncodeData;
  }

  bool Image::SavePNG(const char* _Directory)const
  {
    assert(!isEmpty());

    const std::time_t Time = std::time(nullptr);
    tm Tm;
    localtime_s(&Tm, &Time);

    const std::filesystem::path Directory = std::filesystem::path(_Directory);
    {
      if (!std::filesystem::exists(Directory))
      {
        if (!std::filesystem::create_directory(Directory))
        {
          return false;
        }
      }
    }

    std::filesystem::path FilePath;
    {
      const std::string Str = std::to_string(Tm.tm_year + 1900) + std::to_string(Tm.tm_mon) + std::to_string(Tm.tm_mday)
        + std::to_string(Tm.tm_hour) + std::to_string(Tm.tm_min) + std::to_string(Tm.tm_sec);
      for (vdl::uint i = 0;; ++i)
      {
        FilePath = Directory / (Str + std::to_string(i) + ".png");
        if (!std::filesystem::exists(FilePath))
        {
          break;
        }
      }
    }

    const std::vector<uint8_t> PNG = EncodePNG();

    std::ofstream OFStream(FilePath.string().c_str(), std::ios::out | std::ios::binary);
    assert(OFStream);
    OFStream.write(reinterpret_cast<const char*>(PNG.data()), sizeof(uint8_t)*PNG.size());
    OFStream.close();

    return true;
  }

  //--------------------------------------------------

  CompressionImage::CompressionImage(const Image& _Image)
    : Size_(_Image.GetSize())
  {
    Buffer_ = _Image.EncodePNG();
  }
}
