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
  std::filesystem::path BinaryFilePath = std::filesystem::path(std::string(Constants::kBinaryFileDirectory) + std::string(_FilePath)).concat(Constants::kBinaryFileFormat);
  std::filesystem::path BinaryFileDirectory = BinaryFilePath.remove_filename();

  vdl::Image Image;
  if (_isSerialize)
  {
    //  バイナリファイルが存在する場合ファイルから読み込む
    if (std::filesystem::exists(BinaryFilePath))
    {
      ImportFromBinary(BinaryFilePath.string().c_str(), Image);
    }
  }

  if (Image.isEmpty())
  {
    Image = std::move(TextureLoader().LoadFromFile(_FilePath));
  }

  if (_isSerialize)
  {
    //  フォルダが存在しない場合作成
    if (!std::filesystem::exists(BinaryFileDirectory))
    {
      std::filesystem::create_directories(BinaryFileDirectory);
    }

    //  バイナリファイルに書き出し
    ExportToBinary(BinaryFilePath.string().c_str(), Image);
  }

  return Load(Image);
}

vdl::ID CTextureManager::Load(const vdl::Image& _Image)
{
  ITexture* pTexture;
  pDevice_->CreateTexture(&pTexture, _Image);

  return Textures_.Add(pTexture);
}

vdl::ID CTextureManager::CreateRenderTexture(const vdl::uint2& _TextureSize, vdl::Format _Format)
{
  ITexture* pRenderTexture;
  pDevice_->CreateRenderTexture(&pRenderTexture, _TextureSize, _Format);

  return Textures_.Add(pRenderTexture);
}

vdl::ID CTextureManager::CreateDepthStencilTexture(const vdl::uint2& _TextureSize, vdl::Format _Format)
{
  ITexture* pDepthStencilTexture;
  pDevice_->CreateDepthStecilTexture(&pDepthStencilTexture, _TextureSize, _Format);

  return Textures_.Add(pDepthStencilTexture);
}
