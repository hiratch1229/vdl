#include "CModelManager.hpp"

#include "ModelLoader/FBXLoader.hpp"
#include "ModelLoader/glTFLoader.hpp"

#include <ThirdParty/meshoptimizer/meshoptimizer.h>

#include <vdl/Constants/Constants.hpp>
#include <vdl/Misc/Misc.hpp>

#include <vdl/Model.hpp>
#include <vdl/Serialize.hpp>
#include <vdl/DetectMemoryLeak.hpp>

namespace
{
  constexpr IndexType kIndexType = (sizeof(vdl::IndexType) == 2 ? IndexType::eUint16 : IndexType::eUint32);
}

vdl::ID CModelManager::Load(const vdl::VertexStaticMeshs& _Vertices, const vdl::Indices& _Indices, const vdl::StaticMeshData& _StaticMeshData)
{
  const size_t VertexNum = _Vertices.size();
  vdl::VertexSkinnedMeshs Vertices(VertexNum);
  for (size_t VertexCount = 0; VertexCount < VertexNum; ++VertexCount)
  {
    Vertices[VertexCount] = _Vertices[VertexCount];
  }

  return Load(Vertices, _Indices, _StaticMeshData);
}

vdl::ID CModelManager::Load(const vdl::VertexSkinnedMeshs& _Vertices, const vdl::Indices& _Indices, const vdl::SkinnedMeshData& _SkinnedMeshData)
{
  auto GetVertexBuffer = [&]()->VertexBuffer
  {
    for (auto& VertexBufferData : VertexBufferDatas_)
    {
      if (VertexBufferData && VertexBufferData->Vertices == _Vertices)
      {
        return VertexBufferData->VertexBuffer;
      }
    }

    VertexBuffer Buffer = VertexBuffer(_Vertices.data(), static_cast<vdl::uint>(_Vertices.size() * sizeof(vdl::VertexSkinnedMesh)));
    {
      if (VertexBufferDatas_.size() <= Buffer.GetID())
      {
        VertexBufferDatas_.resize(*Buffer.GetID() + 1);
      }
      VertexBufferData* pData = new VertexBufferData;
      {
        pData->VertexBuffer = Buffer;
        pData->Vertices = _Vertices;
      }

      VertexBufferDatas_[*Buffer.GetID()] = pData;
    }

    return Buffer;
  };
  auto GetIndexBuffer = [&]()->IndexBuffer
  {
    for (auto& IndexBufferData : IndexBufferDatas_)
    {
      if (IndexBufferData && IndexBufferData->Indices == _Indices)
      {
        return IndexBufferData->IndexBuffer;
      }
    }

    IndexBuffer Buffer = IndexBuffer(_Indices.data(), static_cast<vdl::uint>(_Indices.size() * sizeof(vdl::IndexType)), kIndexType);
    {
      if (IndexBufferDatas_.size() <= Buffer.GetID())
      {
        IndexBufferDatas_.resize(*Buffer.GetID() + 1);
      }
      IndexBufferData* pData = new IndexBufferData;
      {
        pData->IndexBuffer = Buffer;
        pData->Indices = _Indices;
      }

      IndexBufferDatas_[*Buffer.GetID()] = pData;
    }

    return Buffer;
  };

  Mesh* pMesh = new Mesh;
  {
    pMesh->VertexBuffer = GetVertexBuffer();
    pMesh->IndexBuffer = GetIndexBuffer();
    pMesh->IndexStart = _SkinnedMeshData.IndexStart;
    pMesh->IndexCount = _SkinnedMeshData.IndexCount;
    pMesh->Material = _SkinnedMeshData.Material;
    pMesh->Animations = _SkinnedMeshData.Animations;
    pMesh->GlobalTransform = _SkinnedMeshData.GlobalTransform;
  }

  return Meshes_.Add(pMesh);
}

std::vector<vdl::SkinnedMesh> CModelManager::Load(const char* _FilePath, bool _isSerialize)
{
  const std::filesystem::path OriginalFilePath = _FilePath;
  const std::filesystem::path BinaryFileDirectory = std::filesystem::path(Constants::kBinaryFileDirectory) / std::filesystem::path(_FilePath).remove_filename();
  const std::filesystem::path BinaryFilePath = (BinaryFileDirectory / std::filesystem::path(_FilePath).filename()).concat(Constants::kBinaryFileFormat);
  const bool existOriginalFile = std::filesystem::exists(OriginalFilePath);

  ModelData ModelData;
  {
    //  バイナリファイルが存在して、元ファイルの更新日時が古い場合読み込み
    if (_isSerialize
      && std::filesystem::exists(BinaryFilePath) && !(existOriginalFile && ::isFileUpdate(OriginalFilePath, BinaryFilePath)))
    {
      ::ImportFromBinary(BinaryFilePath.string().c_str(), ModelData);
    }
    else
    {
      _ASSERT_EXPR_A(existOriginalFile, std::string(std::string(_FilePath) + "が見つかりません。").c_str());

      const std::string FileFormat = GetFileFormat(_FilePath);

      if (FBXLoader::CheckSupportFormat(FileFormat))
      {
        ModelData = FBXLoader().Load(_FilePath);
      }
      else if (glTFLoader::CheckSupportFormat(FileFormat))
      {
        ModelData = glTFLoader().Load(_FilePath);
      }
      else
      {
        _ASSERT_EXPR_A(false, (FileFormat + "は対応していません。").c_str());
      }

      //  メッシュの最適化
      {
        const size_t VertexNum = ModelData.Vertices.size();
        const size_t IndexNum = ModelData.Indices.size();

        std::vector<vdl::IndexType> Remap(VertexNum);
        const size_t NewVertexNum = ::meshopt_generateVertexRemap(Remap.data(), ModelData.Indices.data(), IndexNum, ModelData.Vertices.data(), VertexNum, sizeof(vdl::VertexSkinnedMesh));

        vdl::Indices Indices(IndexNum);
        ::meshopt_remapIndexBuffer(Indices.data(), ModelData.Indices.data(), IndexNum, Remap.data());

        vdl::VertexSkinnedMeshs Vertices(NewVertexNum);
        ::meshopt_remapVertexBuffer(Vertices.data(), ModelData.Vertices.data(), VertexNum, sizeof(vdl::VertexSkinnedMesh), Remap.data());

        ModelData.Vertices = std::move(Vertices);
        ModelData.Indices = std::move(Indices);
      }

      if (_isSerialize)
      {
        //  フォルダが存在しない場合作成
        if (!std::filesystem::exists(BinaryFileDirectory))
        {
          std::filesystem::create_directories(BinaryFileDirectory);
        }

        //  バイナリファイルに書き出し
        ::ExportToBinary(BinaryFilePath.string().c_str(), ModelData);
      }
    }
  }

  //  マテリアルデータの変換
  vdl::Materials Materials;
  {
    auto CreateTexture = [](const vdl::CompressionImage& _CompressionImage, const vdl::Color4& _DummyData)->vdl::Image
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

    for (auto& ModelMaterial : ModelData.Materials)
    {
      vdl::Material& Material = Materials.emplace_back();
      Material.MaterialColor = ModelMaterial.MaterialColor;
      Material.Diffuse = CreateTexture(ModelMaterial.Diffuse, vdl::Palette::Diffuse);
      Material.NormalMap = CreateTexture(ModelMaterial.NormalMap, vdl::Palette::NormalMap);
      //Material.Specular = CreateTexture(ModelMaterial.Specular, vdl::Color(0, 0, 0));
      //Material.MetalicRoughness = CreateTexture(ModelMaterial.MetalicRoughness, vdl::Color(0, 0, 0));
      //Material.Emissive = CreateTexture(ModelMaterial.Emissive, vdl::Color(0, 0, 0));
    }
  }

  std::vector<vdl::SkinnedMesh> Meshes;
  {
    const size_t MeshNum = ModelData.MeshDatas.size();
    Meshes.resize(MeshNum);

    for (size_t MeshCount = 0; MeshCount < MeshNum; ++MeshCount)
    {
      const MeshData& ModelMesh = ModelData.MeshDatas[MeshCount];

      vdl::SkinnedMeshData MeshData;
      {
        MeshData.IndexStart = ModelMesh.IndexStart;
        MeshData.IndexCount = ModelMesh.IndexCount;
        MeshData.Material = Materials[ModelMesh.MaterialIndex];
        MeshData.GlobalTransform = ModelMesh.GlobalTransform;
        //MeshData.Animations = ModelMesh.Animations;
      }

      Meshes[MeshCount] = vdl::SkinnedMesh(ModelData.Vertices, ModelData.Indices, MeshData);
    }
  }

  return Meshes;
}
