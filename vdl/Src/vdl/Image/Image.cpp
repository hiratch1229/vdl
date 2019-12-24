#include <vdl/Image.hpp>

#include <vdl/Misc/Misc.hpp>

#include <ThirdParty/zstd/zstd.h>
#include <ThirdParty/zstd/common/zstd_errors.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ThirdParty/stb/stb_image_write.h>

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace vdl
{
  Image::Image(const CompressionImage& _CompressionImage)
    : Size_(_CompressionImage.GetTextureSize())
  {
    Resize(Size_);

    size_t Result;
    Result = ::ZSTD_decompress(Buffer_.data(), BufferSize(), _CompressionImage.Buffer(), _CompressionImage.BufferSize());
    _ASSERT_EXPR_A(!::ZSTD_isError(Result), ::ZSTD_getErrorString(::ZSTD_getErrorCode(Result)));
  }

  std::vector<uint8_t> Image::EncodePNG()const
  {
    assert(!isEmpty());

    std::vector<uint8_t> EncodeData;
    {
      int Lenght;
      uint8_t* PNG = ::stbi_write_png_to_mem(&Buffer()->Red, static_cast<int>(Size_.x * 4), static_cast<int>(Size_.x), static_cast<int>(Size_.y), sizeof(Color4), &Lenght);

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
      std::string Str;
      {
        std::stringstream SStream;
        SStream << std::setw(4) << Tm.tm_year + 1900 << std::setw(2) << Tm.tm_mon << std::setw(2) << Tm.tm_mday
          << std::setw(2) << Tm.tm_hour << std::setw(2) << Tm.tm_min << std::setw(2) << Tm.tm_sec;

        Str = SStream.str();
      }

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
    : Size_(_Image.GetTextureSize())
  {
    const size_t SrcBufferSize = static_cast<size_t>(_Image.BufferSize());
    const size_t DstBufferSize = ::ZSTD_compressBound(SrcBufferSize);
    Buffer_.resize(DstBufferSize);

    size_t Result;
    Result = ::ZSTD_compress(Buffer_.data(), DstBufferSize, _Image.Buffer(), SrcBufferSize, ZSTD_CLEVEL_DEFAULT);
    _ASSERT_EXPR_A(!::ZSTD_isError(Result), ::ZSTD_getErrorString(::ZSTD_getErrorCode(Result)));

    Buffer_.resize(Result);
    Buffer_.shrink_to_fit();
  }
}
