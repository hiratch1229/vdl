#include "TextureLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_NO_PIC
#define STBI_NO_PSD
#define STBI_NO_PNM
#include <ThirdParty/stb/stb_image.h>

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

vdl::Image TextureLoader::LoadFromMemory(const std::vector<vdl::uint8_t>& _Buffer)const
{
  vdl::Image Image;
  {
    vdl::int2 TextureSize;
    int Comp;
    void* pData = ::stbi_load_from_memory(_Buffer.data(), static_cast<int>(_Buffer.size()), &TextureSize.x, &TextureSize.y, &Comp, 4);

    Image.Resize(TextureSize);
    ::memcpy(Image.Buffer(), pData, Image.BufferSize());

    ::stbi_image_free(pData);
  }

  return Image;
}

//#include "TextureLoader.hpp"
//
//#include <vdl/Misc/Windows/Misc.hpp>
//
//TextureLoader::TextureLoader()
//{
//  HRESULT hr = S_OK;
//
//  //  COMÇÃèâä˙âª
//  hr = ::CoInitialize(nullptr);
//  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
//
//  hr = ::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(pFactory_.GetAddressOf()));
//  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
//}
//
//vdl::Image TextureLoader::LoadFromFile(const char* _FilePath)const
//{
//  HRESULT hr = S_OK;
//
//  //  charå^Å®wchar_tå^Ç…ïœä∑
//  wchar_t wFilePath[256];
//  ::mbstowcs_s(nullptr, wFilePath, _FilePath, 256);
//
//  Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder;
//  hr = pFactory_->CreateDecoderFromFilename(wFilePath, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, pDecoder.GetAddressOf());
//  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
//
//  return GetImage(pDecoder.Get());
//}
//
//vdl::Image TextureLoader::LoadFromMemory(const std::vector<vdl::uint8_t>& _Buffer)const
//{
//  HRESULT hr = S_OK;
//
//  Microsoft::WRL::ComPtr<IWICStream> pStream;
//  hr = pFactory_->CreateStream(pStream.GetAddressOf());
//  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
//
//  hr = pStream->InitializeFromMemory(const_cast<vdl::uint8_t*>(_Buffer.data()), static_cast<DWORD>(_Buffer.size()));
//  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
//
//  Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder;
//  hr = pFactory_->CreateDecoderFromStream(pStream.Get(), 0, WICDecodeMetadataCacheOnDemand, pDecoder.GetAddressOf());
//  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
//
//  return GetImage(pDecoder.Get());
//}
//
////--------------------------------------------------
//
//vdl::Image TextureLoader::GetImage(IWICBitmapDecoder* _pDecoder)const
//{
//  HRESULT hr = S_OK;
//
//  vdl::Image Image;
//
//  Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pFrame;
//  hr = _pDecoder->GetFrame(0, pFrame.GetAddressOf());
//  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
//
//  vdl::uint2 Size;
//  hr = pFrame->GetSize(&Size.x, &Size.y);
//  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
//
//  Image.Resize(Size);
//
//  WICPixelFormatGUID PixelFormat;
//  hr = pFrame->GetPixelFormat(&PixelFormat);
//  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
//
//  if (PixelFormat != GUID_WICPixelFormat32bppRGBA)
//  {
//    Microsoft::WRL::ComPtr<IWICFormatConverter> pConverter;
//
//    hr = pFactory_->CreateFormatConverter(pConverter.GetAddressOf());
//    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
//
//    hr = pConverter->Initialize(pFrame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeErrorDiffusion, nullptr, 0, WICBitmapPaletteTypeCustom);
//    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
//
//    hr = pConverter->CopyPixels(0, Image.Stride(), Image.BufferSize(), reinterpret_cast<BYTE*>(Image.Buffer()));
//    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
//  }
//  else
//  {
//    hr = pFrame->CopyPixels(0, Image.Stride(), Image.BufferSize(), reinterpret_cast<BYTE*>(Image.Buffer()));
//    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
//  }
//
//  return Image;
//}
