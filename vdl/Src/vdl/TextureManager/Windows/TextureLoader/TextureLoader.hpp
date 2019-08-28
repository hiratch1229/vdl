#pragma once
#include <vdl/Image.hpp>

#include <wrl.h>
#include <wincodec.h>

class TextureLoader
{
  Microsoft::WRL::ComPtr<IWICImagingFactory> pFactory_;
private:
  vdl::Image GetImage(IWICBitmapDecoder* _pDecoder)const;
public:
  TextureLoader();

  ~TextureLoader() = default;

  vdl::Image LoadFromFile(const char* _FilePath)const;

  vdl::Image LoadFromMemory(const std::vector<vdl::uint8_t>& _Buffer)const;
};
