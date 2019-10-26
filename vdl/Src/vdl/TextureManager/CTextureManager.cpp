#include "CTextureManager.hpp"

#include "TextureLoader/TextureLoader.hpp"

#include <vdl/Image.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/Constants/Constants.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <filesystem>

void CTextureManager::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
}

vdl::ID CTextureManager::Load(const char* _FilePath, bool _isSerialize)
{
  const std::filesystem::path BinaryFileDirectory = std::filesystem::path(Constants::kBinaryFileDirectory) / std::filesystem::path(_FilePath).remove_filename();
  const std::filesystem::path BinaryFilePath = (BinaryFileDirectory / std::filesystem::path(_FilePath).filename()).concat(Constants::kBinaryFileFormat);

  vdl::Image Image;
  if (_isSerialize)
  {
    //  バイナリファイルが存在する場合ファイルから読み込む
    if (std::filesystem::exists(BinaryFilePath))
    {
      vdl::CompressionImage CompressionImage;
      ImportFromBinary(BinaryFilePath.string().c_str(), CompressionImage);
      Image = CompressionImage;
    }
  }

  if (Image.isEmpty())
  {
    _ASSERT_EXPR_A(std::filesystem::exists(std::filesystem::path(_FilePath)),
      std::string(std::string(_FilePath) + "が見つかりません。").c_str());

    Image = TextureLoader().LoadFromFile(_FilePath);

    if (_isSerialize)
    {
      //  フォルダが存在しない場合作成
      if (!std::filesystem::exists(BinaryFileDirectory))
      {
        std::filesystem::create_directories(BinaryFileDirectory);
      }

      //  バイナリファイルに書き出し
      vdl::CompressionImage CompressionImage = Image;
      ExportToBinary(BinaryFilePath.string().c_str(), CompressionImage);
    }
  }

  return Load(Image);
}

vdl::ID CTextureManager::Load(const vdl::Image& _Image)
{
  ITexture* pTexture;
  pDevice_->CreateTexture(&pTexture, _Image);

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
  pDevice_->CreateDepthStecilTexture(&pDepthStencilTexture, _TextureSize, _Format);

  return Textures_.Add(pDepthStencilTexture);
}

vdl::ID CTextureManager::CreateUnorderedAccessTexture(const vdl::uint2& _TextureSize, vdl::FormatType _Format)
{
  ITexture* pUnorderedAccessTexture;
  pDevice_->CreateUnorderedAccessTexture(&pUnorderedAccessTexture, _TextureSize, _Format);

  return Textures_.Add(pUnorderedAccessTexture);
}
