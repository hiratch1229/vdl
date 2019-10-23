#include "CModelManager.hpp"

#include <vdl/Model.hpp>

#include <vdl/Constants/Constants.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include "ModelLoader/FBXLoader.hpp"
#include "ModelLoader/glTFLoader.hpp"

#include <filesystem>

namespace
{
  constexpr IndexType kIndexType = (sizeof(vdl::IndexType) == 2 ? IndexType::eUint16 : IndexType::eUint32);
}

vdl::ID CModelManager::Load(const vdl::MeshData& _MeshData)
{
  Mesh* pMesh = new Mesh;
  {
    pMesh->VertexBuffer = VertexBuffer(_MeshData.Vertices.data(), sizeof(vdl::Vertex3D), static_cast<vdl::uint>(_MeshData.Vertices.size() * sizeof(vdl::Vertex3D)));
    pMesh->IndexBuffer = IndexBuffer(_MeshData.Indices.data(), static_cast<vdl::uint>(_MeshData.Indices.size() * sizeof(vdl::IndexType)), kIndexType);
    pMesh->Name = _MeshData.Name;
    pMesh->Material = _MeshData.Material;
    pMesh->GlobalTransform = _MeshData.GlobalTransform;
  }

  return Meshes_.Add(pMesh);
}

vdl::ID CModelManager::Load(vdl::MeshData&& _MeshData)
{
  Mesh* pMesh = new Mesh;
  {
    pMesh->VertexBuffer = VertexBuffer(_MeshData.Vertices.data(), sizeof(vdl::Vertex3D), static_cast<vdl::uint>(_MeshData.Vertices.size() * sizeof(vdl::Vertex3D)));
    pMesh->IndexBuffer = IndexBuffer(_MeshData.Indices.data(), static_cast<vdl::uint>(_MeshData.Indices.size() * sizeof(vdl::IndexType)), kIndexType);
    pMesh->Name = std::move(_MeshData.Name);
    pMesh->Material = std::move(_MeshData.Material);
    pMesh->GlobalTransform = std::move(_MeshData.GlobalTransform);
  }

  return Meshes_.Add(pMesh);
}

std::vector<vdl::Mesh> CModelManager::Load(const char* _FilePath, bool _isSerialize)
{
  const std::filesystem::path BinaryFileDirectory = std::filesystem::path(Constants::kBinaryFileDirectory) / std::filesystem::path(_FilePath).remove_filename();
  const std::filesystem::path BinaryFilePath = (BinaryFileDirectory / std::filesystem::path(_FilePath).filename()).concat(Constants::kBinaryFileFormat);

  MeshDatas MeshDatas;
  {
    const std::string FileFormat = GetFileFormat(_FilePath);

    if (_isSerialize)
    {
      //  バイナリファイルが存在する場合ファイルから読み込む
      if (std::filesystem::exists(BinaryFilePath))
      {
        ImportFromBinary(BinaryFilePath.string().c_str(), MeshDatas);
      }
    }

    if (MeshDatas.empty())
    {
      _ASSERT_EXPR_A(std::filesystem::exists(std::filesystem::path(_FilePath)),
        std::string(std::string(_FilePath) + "が見つかりません。").c_str());

      if (FBXLoader::CheckSupportFormat(FileFormat))
      {
        MeshDatas = FBXLoader().Load(_FilePath);
      }
      //else if (glTFLoader::CheckSupportFormat(FileFormat))
      //{
      //  MeshDatas = glTFLoader().Load(_FilePath);
      //}
      else
      {
        _ASSERT_EXPR_A(false, (FileFormat + "は対応していません。").c_str());
      }

      if (_isSerialize)
      {
        //  フォルダが存在しない場合作成
        if (!std::filesystem::exists(BinaryFileDirectory))
        {
          std::filesystem::create_directories(BinaryFileDirectory);
        }

        //  バイナリファイルに書き出し
        ExportToBinary(BinaryFilePath.string().c_str(), MeshDatas);
      }
    }
  }

  auto GetImage = [](const vdl::CompressionImage& _CompressionImage, const vdl::ColorF& _DummyData)->vdl::Image
  {
    vdl::Image Image;
    {
      if (_CompressionImage.isEmpty())
      {
        Image.Resize(1);
        Image.Buffer()[0] = _DummyData;
      }
      else
      {
        Image = _CompressionImage;
      }
    }

    return Image;
  };

  std::vector<vdl::Mesh> SkinnedMeshes;

  const vdl::uint MeshNum = static_cast<vdl::uint>(MeshDatas.size());
  for (vdl::uint MeshCount = 0; MeshCount < MeshNum; ++MeshCount)
  {
    MeshData& LoadMeshData = MeshDatas[MeshCount];

    vdl::MeshData MeshData;
    {
      MeshData.Name = std::move(LoadMeshData.Name);
      MeshData.Vertices = std::move(LoadMeshData.Vertices);
      MeshData.GlobalTransform = std::move(LoadMeshData.GlobalTransform);
      Indices Indices = std::move(LoadMeshData.Indices);

      const size_t AnimationNum = LoadMeshData.Animations.size();
      MeshData.Animations.resize(AnimationNum);
      for (size_t AnimationCount = 0; AnimationCount < AnimationNum; ++AnimationCount)
      {
        Animation& LoadAnimation = LoadMeshData.Animations[AnimationCount];
        vdl::Animation& Animation = MeshData.Animations[AnimationCount];

        const size_t SkeletalNum = LoadAnimation.Skeletals.size();
        Animation.Skeletals.resize(SkeletalNum);
        for (size_t SkeletalCount = 0; SkeletalCount < SkeletalNum; ++SkeletalCount)
        {
          Skeletal& LoadSkeletal = LoadAnimation.Skeletals[SkeletalCount];
          vdl::Skeletal& Skeletal = Animation.Skeletals[SkeletalCount];

          for (auto& Bone : LoadSkeletal)
          {
            Skeletal.Bones[Bone.Name].Pose = std::move(Bone.Pose);
            Skeletal.Bones[Bone.Name].Offset = std::move(Bone.Offset);
          }
        }
      }

      const size_t MaterialNum = LoadMeshData.Materials.size();
      for (size_t MaterialCount = 0; MaterialCount < MaterialNum; ++MaterialCount)
      {
        Material& LoadMaterial = LoadMeshData.Materials[MaterialCount];

        MeshData.Material.MaterialColor = LoadMaterial.MaterialColor;
        MeshData.Material.Diffuse = GetImage(LoadMaterial.Diffuse, vdl::ColorF(1.0f, 1.0f, 1.0f));
        MeshData.Material.NormalMap = GetImage(LoadMaterial.NormalMap, vdl::ColorF(0.5f, 0.5f, 1.0f));
        //Material.Specular = GetImage(LoadMaterial.Specular, vdl::ColorF(0.0f, 0.0f, 0.0f));
        //Material.MetallicRoughness = GetImage(LoadMaterial.MetallicRoughness, vdl::ColorF(1.0f, 1.0f, 1.0f));
        //Material.Emissive = GetImage(LoadMaterial.Emissive, vdl::ColorF(0.0f, 0.0f, 0.0f));

        MeshData.Material.IndexNum = LoadMaterial.IndexCount - LoadMaterial.IndexStart;
        MeshData.Indices.resize(MeshData.Material.IndexNum);
        ::memcpy(MeshData.Indices.data(), &Indices[LoadMaterial.IndexStart], sizeof(vdl::IndexType) * MeshData.Material.IndexNum);

        SkinnedMeshes.push_back(MeshData);
      }
    }
  }

  return SkinnedMeshes;
}
