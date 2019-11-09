#include "CTextureManager.hpp"

#include "TextureLoader/TextureLoader.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/Constants/Constants.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/Image.hpp>
#include <vdl/DetectMemoryLeak.hpp>

#include <filesystem>

void CTextureManager::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
}

vdl::ID CTextureManager::LoadTexture(const char* _FilePath, bool _isSerialize)
{
  return LoadTexture(GetImageFromFilePath(_FilePath, _isSerialize));
}

vdl::ID CTextureManager::LoadTexture(const vdl::Image& _Image)
{
  ITexture* pTexture;
  pDevice_->CreateTexture(&pTexture, _Image);

  return Textures_.Add(pTexture);
}

vdl::ID CTextureManager::LoadCubeTexture(const char* _FilePath, bool _isSerialize)
{
  return LoadCubeTexture(GetImageFromFilePath(_FilePath, _isSerialize));
}

vdl::ID CTextureManager::LoadCubeTexture(const vdl::Image& _Image)
{
  ITexture* pTexture;
  pDevice_->CreateCubeTexture(&pTexture, _Image);

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

//--------------------------------------------------

vdl::Image CTextureManager::GetImageFromFilePath(const char* _FilePath, bool _isSerialize)const
{
  const std::filesystem::path BinaryFileDirectory = std::filesystem::path(Constants::kBinaryFileDirectory) / std::filesystem::path(_FilePath).remove_filename();
  const std::filesystem::path BinaryFilePath = (BinaryFileDirectory / std::filesystem::path(_FilePath).filename()).concat(Constants::kBinaryFileFormat);

  vdl::Image Image;
  if (_isSerialize)
  {
    //  �o�C�i���t�@�C�������݂���ꍇ�t�@�C������ǂݍ���
    if (std::filesystem::exists(BinaryFilePath))
    {
      vdl::CompressionImage CompressionImage;
      ::ImportFromBinary(BinaryFilePath.string().c_str(), CompressionImage);
      Image = CompressionImage;
    }
  }

  if (Image.isEmpty())
  {
    _ASSERT_EXPR_A(std::filesystem::exists(std::filesystem::path(_FilePath)),
      std::string(std::string(_FilePath) + "��������܂���B").c_str());

    Image = TextureLoader().LoadFromFile(_FilePath);

    if (_isSerialize)
    {
      //  �t�H���_�����݂��Ȃ��ꍇ�쐬
      if (!std::filesystem::exists(BinaryFileDirectory))
      {
        std::filesystem::create_directories(BinaryFileDirectory);
      }

      //  �o�C�i���t�@�C���ɏ����o��
      vdl::CompressionImage CompressionImage = Image;
      ::ExportToBinary(BinaryFilePath.string().c_str(), CompressionImage);
    }
  }

  return Image;
}
