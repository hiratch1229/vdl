#include "CModelManager.hpp"

#include <vdl/Model.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/Constants/Constants.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include "ModelLoader/FBXLoader.hpp"
#include "ModelLoader/glTFLoader.hpp"

#include <filesystem>

namespace
{
  constexpr IndexType kIndexType = (sizeof(vdl::IndexType) == 2 ? IndexType::eUint16 : IndexType::eUint32);
}

void CModelManager::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
}

vdl::ID CModelManager::Load(const vdl::StaticMeshData& _MeshData)
{
  Mesh* pMesh = new Mesh;
  {
    IBuffer* pVertexBuffer;
    pDevice_->CreateVertexBuffer(&pVertexBuffer, _MeshData.Vertices.data(), sizeof(vdl::StaticMeshVertex), static_cast<vdl::uint>(_MeshData.Vertices.size() * sizeof(vdl::StaticMeshVertex)));
    pMesh->pVertexBuffer.reset(pVertexBuffer);

    IBuffer* pIndexBuffer;
    pDevice_->CreateIndexBuffer(&pIndexBuffer, _MeshData.Indices.data(), static_cast<vdl::uint>(_MeshData.Indices.size() * sizeof(vdl::IndexType)), kIndexType);
    pMesh->pIndexBuffer.reset(pIndexBuffer);

    pMesh->Name = _MeshData.Name;
    pMesh->Materials = _MeshData.Materials;
    pMesh->GlobalTransform = _MeshData.GlobalTransform;
  }

  return Meshes_.Add(pMesh);
}

vdl::ID CModelManager::Load(vdl::StaticMeshData&& _MeshData)
{
  Mesh* pMesh = new Mesh;
  {
    IBuffer* pVertexBuffer;
    pDevice_->CreateVertexBuffer(&pVertexBuffer, _MeshData.Vertices.data(), sizeof(vdl::StaticMeshVertex), static_cast<vdl::uint>(_MeshData.Vertices.size() * sizeof(vdl::StaticMeshVertex)));
    pMesh->pVertexBuffer.reset(pVertexBuffer);

    IBuffer* pIndexBuffer;
    pDevice_->CreateIndexBuffer(&pIndexBuffer, _MeshData.Indices.data(), static_cast<vdl::uint>(_MeshData.Indices.size() * sizeof(vdl::IndexType)), kIndexType);
    pMesh->pIndexBuffer.reset(pIndexBuffer);

    pMesh->Name = std::move(_MeshData.Name);
    pMesh->Materials = std::move(_MeshData.Materials);
    pMesh->GlobalTransform = std::move(_MeshData.GlobalTransform);
  }

  return Meshes_.Add(pMesh);
}

vdl::ID CModelManager::Load(const vdl::SkinnedMeshData& _MeshData)
{
  Mesh* pMesh = new Mesh;
  {
    IBuffer* pVertexBuffer;
    pDevice_->CreateVertexBuffer(&pVertexBuffer, _MeshData.Vertices.data(), sizeof(vdl::SkinnedMeshVertex), static_cast<vdl::uint>(_MeshData.Vertices.size() * sizeof(vdl::SkinnedMeshVertex)));
    pMesh->pVertexBuffer.reset(pVertexBuffer);

    IBuffer* pIndexBuffer;
    pDevice_->CreateIndexBuffer(&pIndexBuffer, _MeshData.Indices.data(), static_cast<vdl::uint>(_MeshData.Indices.size() * sizeof(vdl::IndexType)), kIndexType);
    pMesh->pIndexBuffer.reset(pIndexBuffer);

    pMesh->Name = _MeshData.Name;
    pMesh->Materials = _MeshData.Materials;
    pMesh->Animations = _MeshData.Animations;
    pMesh->GlobalTransform = _MeshData.GlobalTransform;
  }

  return Meshes_.Add(pMesh);
}

vdl::ID CModelManager::Load(vdl::SkinnedMeshData&& _MeshData)
{
  Mesh* pMesh = new Mesh;
  {
    IBuffer* pVertexBuffer;
    pDevice_->CreateVertexBuffer(&pVertexBuffer, _MeshData.Vertices.data(), sizeof(vdl::SkinnedMeshVertex), static_cast<vdl::uint>(_MeshData.Vertices.size() * sizeof(vdl::SkinnedMeshVertex)));
    pMesh->pVertexBuffer.reset(pVertexBuffer);

    IBuffer* pIndexBuffer;
    pDevice_->CreateIndexBuffer(&pIndexBuffer, _MeshData.Indices.data(), static_cast<vdl::uint>(_MeshData.Indices.size() * sizeof(vdl::IndexType)), kIndexType);
    pMesh->pIndexBuffer.reset(pIndexBuffer);

    pMesh->Name = std::move(_MeshData.Name);
    pMesh->Materials = std::move(_MeshData.Materials);
    pMesh->Animations = std::move(_MeshData.Animations);
    pMesh->GlobalTransform = std::move(_MeshData.GlobalTransform);
  }

  return Meshes_.Add(pMesh);
}

std::vector<vdl::SkinnedMesh> CModelManager::Load(const char* _FilePath, bool _isSerialize)
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
      if (FBXLoader::CheckSupportFormat(FileFormat))
      {
        MeshDatas = FBXLoader().Load(_FilePath);
      }
      else if (glTFLoader::CheckSupportFormat(FileFormat))
      {
        MeshDatas = glTFLoader().Load(_FilePath);
      }
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

  const vdl::uint MeshNum = static_cast<vdl::uint>(MeshDatas.size());
  std::vector<vdl::SkinnedMesh> SkinnedMeshes(MeshNum);

  for (vdl::uint MeshCount = 0; MeshCount < MeshNum; ++MeshCount)
  {
    MeshData& MeshData = MeshDatas[MeshCount];

    vdl::SkinnedMeshData SkinnedMeshData;
    {
      SkinnedMeshData.Name = std::move(MeshData.Name);
      SkinnedMeshData.Vertices = std::move(MeshData.Vertices);
      SkinnedMeshData.Indices = std::move(MeshData.Indices);
      SkinnedMeshData.GlobalTransform = std::move(MeshData.GlobalTransform);

      const size_t MaterialNum = MeshData.Materials.size();
      SkinnedMeshData.Materials.resize(MaterialNum);
      for (size_t MaterialCount = 0; MaterialCount < MaterialNum; ++MaterialCount)
      {
        vdl::Material& SkinnedMeshMaterial = SkinnedMeshData.Materials[MaterialCount];
        Material& Material = MeshData.Materials[MaterialCount];

        SkinnedMeshMaterial.Diffuse = vdl::Image(Material.Diffuse.CompressionImage);

        SkinnedMeshMaterial.IndexStart = Material.IndexStart;
        SkinnedMeshMaterial.IndexCount = Material.IndexCount;
      }

      const size_t AnimationNum = MeshData.Animations.size();
      SkinnedMeshData.Animations.resize(AnimationNum);
      for (size_t AnimationCount = 0; AnimationCount < AnimationNum; ++AnimationCount)
      {
        vdl::Animation& SkinnedMeshAnimation = SkinnedMeshData.Animations[AnimationCount];
        Animation& Animation = MeshData.Animations[AnimationCount];

        const size_t SkeletalNum = Animation.Skeletals.size();
        SkinnedMeshAnimation.Skeletals.resize(SkeletalNum);
        for (size_t SkeletalCount = 0; SkeletalCount < SkeletalNum; ++SkeletalCount)
        {
          vdl::Skeletal& SkinnedMeshSkeletal = SkinnedMeshAnimation.Skeletals[SkeletalCount];
          Skeletal& Skeletal = Animation.Skeletals[SkeletalCount];

          for (auto& Bone : Skeletal)
          {
            SkinnedMeshSkeletal.Bones[Bone.Name].Pose = std::move(Bone.Pose);
            SkinnedMeshSkeletal.Bones[Bone.Name].Offset = std::move(Bone.Offset);
          }
        }
      }
    }

    SkinnedMeshes[MeshCount] = SkinnedMeshData;
  }

  return SkinnedMeshes;
}
