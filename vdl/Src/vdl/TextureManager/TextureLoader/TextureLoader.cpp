#include "TextureLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_NO_PIC
#define STBI_NO_PSD
#define STBI_NO_PNM
#include <stb/stb_image.h>

#include <vdl/Misc/Misc.hpp>

bool TextureLoader::CheckSupportFormat(const std::string& _FileFormat)
{
  return _FileFormat == "jpg" || _FileFormat == "jpeg" || _FileFormat == "png"
    || _FileFormat == "bmp" || _FileFormat == "tga" || _FileFormat == "gif";
}

//--------------------------------------------------

vdl::Image TextureLoader::LoadFromFile(const char* _FilePath)const
{
  const std::string FileFormat = GetFileFormat(_FilePath);
  _ASSERT_EXPR_A(CheckSupportFormat(FileFormat), (FileFormat + "ÇÕëŒâûÇµÇƒÇ¢Ç‹ÇπÇÒÅB").c_str());

  vdl::Image Image;
  {
    vdl::int2 TextureSize;
    int Comp;
    void* pData = ::stbi_load(_FilePath, &TextureSize.x, &TextureSize.y, &Comp, 4);

    Image.Resize(TextureSize);
    ::memcpy(Image.Buffer(), pData, Image.BufferSize());

    ::stbi_image_free(pData);
  }

  return Image;
}
