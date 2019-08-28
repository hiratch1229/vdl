#include "TextureLoader.hpp"

#include <vdl/Misc/Windows/Misc.hpp>

TextureLoader::TextureLoader()
{
  HRESULT hr = S_OK;

  hr = ::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(pFactory_.GetAddressOf()));
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
}

vdl::Image TextureLoader::LoadFromFile(const char* _FilePath)const
{
  HRESULT hr = S_OK;

  //  charå^Å®wchar_tå^Ç…ïœä∑
  wchar_t wFilePath[256];
  mbstowcs_s(nullptr, wFilePath, _FilePath, 256);

  Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder;
  hr = pFactory_->CreateDecoderFromFilename(wFilePath, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, pDecoder.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  return GetImage(pDecoder.Get());
}

vdl::Image TextureLoader::LoadFromMemory(const std::vector<vdl::uint8_t>& _Buffer)const
{
  HRESULT hr = S_OK;

  Microsoft::WRL::ComPtr<IWICStream> pStream;
  hr = pFactory_->CreateStream(pStream.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  hr = pStream->InitializeFromMemory(const_cast<vdl::uint8_t*>(_Buffer.data()), static_cast<DWORD>(_Buffer.size()));
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder;
  hr = pFactory_->CreateDecoderFromStream(pStream.Get(), 0, WICDecodeMetadataCacheOnDemand, pDecoder.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  return GetImage(pDecoder.Get());
}

//--------------------------------------------------

vdl::Image TextureLoader::GetImage(IWICBitmapDecoder* _pDecoder)const
{
  HRESULT hr = S_OK;

  vdl::Image Image;

  Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pFrame;
  hr = _pDecoder->GetFrame(0, pFrame.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  vdl::uint2 Size;
  hr = pFrame->GetSize(&Size.x, &Size.y);
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  Image.Resize(Size);

  WICPixelFormatGUID PixelFormat;
  hr = pFrame->GetPixelFormat(&PixelFormat);
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  if (PixelFormat != GUID_WICPixelFormat32bppRGBA)
  {
    Microsoft::WRL::ComPtr<IWICFormatConverter> pConverter;

    hr = pFactory_->CreateFormatConverter(pConverter.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    hr = pConverter->Initialize(pFrame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeErrorDiffusion, nullptr, 0, WICBitmapPaletteTypeCustom);
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

    hr = pConverter->CopyPixels(0, Image.Stride(), Image.BufferSize(), &Image.Buffer()->R);
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }
  else
  {
    hr = pFrame->CopyPixels(0, Image.Stride(), Image.BufferSize(), &Image.Buffer()->R);
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  return Image;
}
