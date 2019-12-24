#include "CTextureManager.hpp"

#include "TextureLoader/TextureLoader.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/Constants/Constants.hpp>
#include <vdl/Misc/Misc.hpp>

#include <vdl/Image.hpp>
#include <vdl/Serialize.hpp>
#include <vdl/DetectMemoryLeak.hpp>

void CTextureManager::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
}

vdl::ID CTextureManager::LoadTexture(const char* _FilePath)
{
  return LoadTexture(GetImageFromFilePath(_FilePath));
}

vdl::ID CTextureManager::LoadTexture(const vdl::Image& _Image)
{
  ITexture* pTexture;
  pDevice_->CreateTexture(&pTexture, _Image);

  return Textures_.Add(pTexture);
}

vdl::ID CTextureManager::LoadCubeTexture(const char* _FilePath)
{
  return LoadCubeTexture(GetImageFromFilePath(_FilePath));
}

vdl::ID CTextureManager::LoadCubeTexture(const vdl::Image& _Image)
{
  const vdl::uint2 TextureSize = _Image.GetTextureSize() / vdl::uint2(4, 3);

  auto LoadTexture = [&_Image, &TextureSize](vdl::Image* _pImage, const vdl::uint2& _LeftTopPos)->void
  {
    _pImage->Resize(TextureSize);
    vdl::Color4* Buffer = _pImage->Buffer();

    for (vdl::uint y = 0; y < TextureSize.y; ++y)
    {
      for (vdl::uint x = 0; x < TextureSize.x; ++x)
      {
        Buffer[x + y * TextureSize.x] = _Image.GetColor(_LeftTopPos + vdl::uint2(x, y));
      }
    }
  };

  std::array<vdl::Image, 6> Images;
  LoadTexture(&Images[0], TextureSize * vdl::uint2(2, 1)/* Right */);
  LoadTexture(&Images[1], TextureSize * vdl::uint2(0, 1)/* Left */);
  LoadTexture(&Images[2], TextureSize * vdl::uint2(1, 0)/* Top */);
  LoadTexture(&Images[3], TextureSize * vdl::uint2(1, 2)/* Bottom */);
  LoadTexture(&Images[4], TextureSize * vdl::uint2(1, 1)/* Front */);
  LoadTexture(&Images[5], TextureSize * vdl::uint2(3, 1)/* Back */);

  ITexture* pTexture;
  pDevice_->CreateCubeTexture(&pTexture, Images);

  return Textures_.Add(pTexture);
}

vdl::ID CTextureManager::CreateRenderTexture(const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  ITexture* pRenderTexture;
  pDevice_->CreateRenderTexture(&pRenderTexture, _TextureSize, _Format);

  return Textures_.Add(pRenderTexture);
}

vdl::ID CTextureManager::CreateDepthStencilTexture(const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  ITexture* pDepthStencilTexture;
  pDevice_->CreateDepthStencilTexture(&pDepthStencilTexture, _TextureSize, _Format);

  return Textures_.Add(pDepthStencilTexture);
}

vdl::ID CTextureManager::CreateUnorderedAccessTexture(const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  ITexture* pUnorderedAccessTexture;
  pDevice_->CreateUnorderedAccessTexture(&pUnorderedAccessTexture, _TextureSize, _Format);

  return Textures_.Add(pUnorderedAccessTexture);
}

//--------------------------------------------------

vdl::Image CTextureManager::GetImageFromFilePath(const char* _FilePath)const
{
  const std::filesystem::path OriginalFilePath = _FilePath;
  const std::filesystem::path BinaryFileDirectory = std::filesystem::path(Constants::kBinaryFileDirectory) / std::filesystem::path(_FilePath).remove_filename();
  const std::filesystem::path BinaryFilePath = (BinaryFileDirectory / std::filesystem::path(_FilePath).filename()).concat(Constants::kBinaryFileFormat);
  const bool existOriginalFile = std::filesystem::exists(OriginalFilePath);

  vdl::Image Image;
  {
    //  バイナリファイルが存在して、元ファイルの更新日時が古い場合読み込み
    if (std::filesystem::exists(BinaryFilePath) && !(existOriginalFile && ::isFileUpdate(OriginalFilePath, BinaryFilePath)))
    {
      vdl::CompressionImage CompressionImage;
      ::ImportFromBinary(BinaryFilePath.string().c_str(), CompressionImage);
      Image = CompressionImage;
    }
    else
    {
      _ASSERT_EXPR_A(existOriginalFile, std::string(std::string(_FilePath) + "が見つかりません。").c_str());

      Image = TextureLoader().LoadFromFile(_FilePath);

      //  フォルダが存在しない場合作成
      if (!std::filesystem::exists(BinaryFileDirectory))
      {
        std::filesystem::create_directories(BinaryFileDirectory);
      }

      //  バイナリファイルに書き出し
      vdl::CompressionImage CompressionImage = Image;
      ::ExportToBinary(BinaryFilePath.string().c_str(), CompressionImage);
    }
  }

  return Image;
}
