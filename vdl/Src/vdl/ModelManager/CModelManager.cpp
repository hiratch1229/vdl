#include "CModelManager.hpp"

#include "ModelLoader/FBXLoader.hpp"
#include "ModelLoader/glTFLoader.hpp"

#include <vdl/Constants/Constants.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/Model.hpp>
#include <vdl/DetectMemoryLeak.hpp>

#include <filesystem>

namespace
{
  constexpr IndexType kIndexType = (sizeof(vdl::IndexType) == 2 ? IndexType::eUint16 : IndexType::eUint32);
}

vdl::ID CModelManager::Load(const vdl::Vertices& _Vertices, const vdl::Indices& _Indices, const vdl::MeshData& _MeshData)
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

    VertexBuffer Buffer = VertexBuffer(_Vertices.data(), sizeof(vdl::Vertex3D), static_cast<vdl::uint>(_Vertices.size() * sizeof(vdl::Vertex3D)));
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
    pMesh->IndexStart = _MeshData.IndexStart;
    pMesh->IndexCount = _MeshData.IndexCount;
    pMesh->Material = _MeshData.Material;
    //pMesh->Animations = _MeshData.Animations;
    pMesh->GlobalTransform = _MeshData.GlobalTransform;
  }

  return Meshes_.Add(pMesh);
}

std::vector<vdl::Mesh> CModelManager::Load(const char* _FilePath, bool _isSerialize)
{
  const std::filesystem::path BinaryFileDirectory = std::filesystem::path(Constants::kBinaryFileDirectory) / std::filesystem::path(_FilePath).remove_filename();
  const std::filesystem::path BinaryFilePath = (BinaryFileDirectory / std::filesystem::path(_FilePath).filename()).concat(Constants::kBinaryFileFormat);

  ModelData ModelData;
  {
    const std::string FileFormat = GetFileFormat(_FilePath);

    //  バイナリファイルが存在する場合ファイルから読み込む
    if (_isSerialize && std::filesystem::exists(BinaryFilePath))
    {
      ImportFromBinary(BinaryFilePath.string().c_str(), ModelData);
    }
    else
    {
      _ASSERT_EXPR_A(std::filesystem::exists(std::filesystem::path(_FilePath)),
        std::string(std::string(_FilePath) + "が見つかりません。").c_str());

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

      if (_isSerialize)
      {
        //  フォルダが存在しない場合作成
        if (!std::filesystem::exists(BinaryFileDirectory))
        {
          std::filesystem::create_directories(BinaryFileDirectory);
        }

        //  バイナリファイルに書き出し
        ExportToBinary(BinaryFilePath.string().c_str(), ModelData);
      }
    }
  }

  //  マテリアルデータの変換
  vdl::Materials Materials;
  {
    auto CreateTexture = [](const vdl::CompressionImage& _CompressionImage, const vdl::Color& _DummyData)->vdl::Image
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
      Material.Diffuse = CreateTexture(ModelMaterial.Diffuse, vdl::Color(255, 255, 255));
      Material.NormalMap = CreateTexture(ModelMaterial.NormalMap, vdl::Color(128, 128, 255));
      //Material.Specular = CreateTexture(ModelMaterial.Specular, vdl::Color(0, 0, 0));
      //Material.MetalicRoughness = CreateTexture(ModelMaterial.MetalicRoughness, vdl::Color(0, 0, 0));
      //Material.Emissive = CreateTexture(ModelMaterial.Emissive, vdl::Color(0, 0, 0));
    }
  }

  std::vector<vdl::Mesh> Meshes;
  {
    const size_t MeshNum = ModelData.MeshDatas.size();
    Meshes.resize(MeshNum);

    for (size_t MeshCount = 0; MeshCount < MeshNum; ++MeshCount)
    {
      const MeshData& ModelMesh = ModelData.MeshDatas[MeshCount];

      vdl::MeshData MeshData;
      {
        MeshData.IndexStart = ModelMesh.IndexStart;
        MeshData.IndexCount = ModelMesh.IndexCount;
        MeshData.Material = Materials[ModelMesh.MaterialIndex];
        //MeshData.Animations = ModelMesh.Animations;
        MeshData.GlobalTransform = ModelMesh.GlobalTransform;
      }

      Meshes[MeshCount] = vdl::Mesh(ModelData.Vertices, ModelData.Indices, MeshData);
    }
  }

  return Meshes;
}
