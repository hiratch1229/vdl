#include "glTFLoader.hpp"

#include <vdl/TextureManager/Windows/TextureLoader/TextureLoader.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <filesystem>
#include <sstream>

namespace
{
  inline vdl::ColorF Cast(const Microsoft::glTF::Color4& _Color)
  {
    return { _Color.r, _Color.g, _Color.b, _Color.a };
  }

  inline vdl::Matrix Cast(const Microsoft::glTF::Matrix4& _Matrix)
  {
    return { _Matrix.values[0], _Matrix.values[1], _Matrix.values[2], _Matrix.values[3],
     _Matrix.values[4], _Matrix.values[5], _Matrix.values[6], _Matrix.values[7],
     _Matrix.values[8], _Matrix.values[9], _Matrix.values[10], _Matrix.values[11],
     _Matrix.values[12], _Matrix.values[13], _Matrix.values[14], _Matrix.values[15] };
  }

  inline vdl::Matrix GetMatrixFromNode(const Microsoft::glTF::Node& _Node)
  {
    if (_Node.matrix == Microsoft::glTF::Matrix4::IDENTITY)
    {
      return vdl::Matrix::Scale({ _Node.scale.x, _Node.scale.y, _Node.scale.z })
        * vdl::Matrix::Rotate({ _Node.rotation.x, _Node.rotation.y, _Node.rotation.z, _Node.rotation.w })
        * vdl::Matrix::Translate({ _Node.translation.x, _Node.translation.y, _Node.translation.z });
    }
    else
    {
      return Cast(_Node.matrix);
    }
  }

  class GLTFStreamReader : public Microsoft::glTF::IStreamReader
  {
  public:
    GLTFStreamReader(std::experimental::filesystem::path _PathBase)
      : PathBase_(std::move(_PathBase)) {}

    std::shared_ptr<std::istream> GetInputStream(const std::string& _FileName)const override
    {
      const std::experimental::filesystem::path StreamPath = PathBase_ / std::experimental::filesystem::u8path(_FileName);
      const std::shared_ptr<std::ifstream> IStream = std::make_shared<std::ifstream>(StreamPath, std::ios_base::binary);
      assert(IStream && *IStream);

      return IStream;
    }
  private:
    std::experimental::filesystem::path PathBase_;
  };
}

bool glTFLoader::CheckSupportFormat(const std::string& _FileFormat)
{
  return _FileFormat == "gltf" || _FileFormat == "glb" || _FileFormat == "vrm";
}

//--------------------------------------------------

MeshDatas glTFLoader::Load(const char* _FilePath)const
{
  const std::string FileFormat = GetFileFormat(_FilePath);
  _ASSERT_EXPR_A(CheckSupportFormat(FileFormat), (FileFormat + "は対応していません。").c_str());

  static constexpr vdl::Matrix Transform = vdl::Matrix(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

  MeshDatas MeshDatas;

  std::unique_ptr<Microsoft::glTF::GLTFResourceReader> pResourceReader;
  const Microsoft::glTF::Document Document = LoadDocument(_FilePath, FileFormat, &pResourceReader);

  const TextureLoader TextureLoader;
  const std::string Directory = std::filesystem::path(_FilePath).remove_filename().string();

  for (auto& GltfMesh : Document.meshes.Elements())
  {
    MeshData MeshData;
    {
      FetchMaterial(pResourceReader, Document, GltfMesh, Directory, &MeshData.Materials, TextureLoader);
      FetchVerticesAndIndices(pResourceReader, Document, GltfMesh, &MeshData.Vertices, &MeshData.Indices, &MeshData.Materials);

      MeshData.Name = GltfMesh.name;
      MeshData.GlobalTransform = Transform * GetMatrixFromNode(Document.nodes.Get(GltfMesh.id));
    }

    MeshDatas.emplace_back(std::move(MeshData));
  }

  return MeshDatas;
}

//--------------------------------------------------

Microsoft::glTF::Document glTFLoader::LoadDocument(const char* _FilePath, const std::string& _FileFormat, std::unique_ptr<Microsoft::glTF::GLTFResourceReader>* _pResourceReader)const
{
  std::string JsonData;

  const std::experimental::filesystem::path Path = _FilePath;

  std::unique_ptr<GLTFStreamReader> StreamReader = std::make_unique<GLTFStreamReader>(Path.parent_path());
  std::shared_ptr<std::istream> pIStream = StreamReader->GetInputStream(Path.filename().u8string());

  if (_FileFormat == "gltf")
  {
    std::unique_ptr<Microsoft::glTF::GLTFResourceReader> pGltfResouceReader = std::make_unique<Microsoft::glTF::GLTFResourceReader>(std::move(StreamReader));
    std::stringstream SStream;
    SStream << pIStream->rdbuf();
    JsonData = SStream.str();

    *_pResourceReader = std::move(pGltfResouceReader);
  }
  else
  {
    std::unique_ptr<Microsoft::glTF::GLBResourceReader> pGlbResouceReader = std::make_unique<Microsoft::glTF::GLBResourceReader>(std::move(StreamReader), std::move(pIStream));
    JsonData = pGlbResouceReader->GetJson();

    *_pResourceReader = std::move(pGlbResouceReader);
  }
  assert(_pResourceReader);

  return Microsoft::glTF::Deserialize(JsonData);
}

void glTFLoader::FetchMaterial(const std::unique_ptr<Microsoft::glTF::GLTFResourceReader>& _pResourceReader, const Microsoft::glTF::Document& _Document, const Microsoft::glTF::Mesh& _Mesh, const std::string& _Directory, Materials* _pMaterials, const TextureLoader& _TextureLoader)const
{
  assert(_pMaterials);

  const vdl::uint MaterialNum = static_cast<vdl::uint>(_Mesh.primitives.size());
  _pMaterials->resize(MaterialNum);

  auto GetMaterialBinaryData = [&](Material::Property* _pProperty, const std::string& _TextureID, const Microsoft::glTF::Color4& _Color = { 1.0f, 1.0f, 1.0f, 1.0f })->void
  {
    if (!_TextureID.empty())
    {
      const Microsoft::glTF::Texture& Texture = _Document.textures.Get(_TextureID);
      const Microsoft::glTF::Image& Image = _Document.images.Get(Texture.imageId);

      if (!Image.bufferViewId.empty())
      {
        const Microsoft::glTF::BufferView& BufferView = _Document.bufferViews.Get(Image.bufferViewId);
        //_pProperty->Image = _TextureLoader.LoadFromMemory(_pResourceReader->ReadBinaryData<vdl::uint16_t>(_Document, BufferView));
        _pProperty->CompressionImage = _TextureLoader.LoadFromMemory(_pResourceReader->ReadBinaryData<vdl::uint8_t>(_Document, BufferView));
      }
      else
      {
        _pProperty->CompressionImage = _TextureLoader.LoadFromFile((_Directory + Image.uri).c_str());
      }
    }

    _pProperty->Color = Cast(_Color);
  };

  for (vdl::uint MaterialCount = 0; MaterialCount < MaterialNum; ++MaterialCount)
  {
    Material& Material = (*_pMaterials)[MaterialCount];

    const Microsoft::glTF::Material & GltfMaterial = _Document.materials.Get(_Mesh.primitives[MaterialCount].materialId);

    GetMaterialBinaryData(&Material.Diffuse, GltfMaterial.metallicRoughness.baseColorTexture.textureId, GltfMaterial.metallicRoughness.baseColorFactor);
    GetMaterialBinaryData(&Material.NormalMap, GltfMaterial.normalTexture.textureId);
  }
}

void glTFLoader::FetchVerticesAndIndices(const std::unique_ptr<Microsoft::glTF::GLTFResourceReader>& _pResourceReader, const Microsoft::glTF::Document& _Document, const Microsoft::glTF::Mesh& _Mesh, Vertices* _pVertices, Indices* _pIndices, Materials* _pMaterials)const
{
  assert(_pVertices && _pIndices && _pMaterials);

  auto GetData = [&pResourceReader = _pResourceReader, Document = _Document](const Microsoft::glTF::MeshPrimitive& _MeshPrimitive, const std::string& _AccessorName)->std::vector<float>
  {
    std::string AccessorID;
    if (!_MeshPrimitive.TryGetAttributeAccessorId(_AccessorName, AccessorID))
    {
      return {};
    }

    const Microsoft::glTF::Accessor Accessor = Document.accessors.Get(AccessorID);

    return pResourceReader->ReadBinaryData<float>(Document, Accessor);
  };

  vdl::uint Offset = 0;

  const vdl::uint MaterialNum = static_cast<vdl::uint>(_Mesh.primitives.size());
  for (vdl::uint MaterialCount = 0; MaterialCount < MaterialNum; ++MaterialCount)
  {
    const Microsoft::glTF::MeshPrimitive& MeshPrimitive = _Mesh.primitives[MaterialCount];

    //  頂点データの取得
    {
      Vertices Vertices;
      {
        const std::vector<float> Positions = GetData(MeshPrimitive, "POSITION");
        const bool hasPositions = !Positions.empty();
        const std::vector<float> Normals = GetData(MeshPrimitive, "NORMAL");
        const bool hasNormals = !Normals.empty();
        const std::vector<float> Texcoords = GetData(MeshPrimitive, "TEXCOORD_0");
        const bool hasTexcoords = !Texcoords.empty();

        const vdl::uint VertexNum = static_cast<vdl::uint>(Positions.size());
        Vertices.resize(VertexNum);

        const vdl::uint PolygonNum = VertexNum / 3;
        for (vdl::uint PolygonCount = 0; PolygonCount < PolygonNum; ++PolygonCount)
        {
          vdl::SkinnedMeshVertex& Vertex = Vertices[PolygonCount];

          if (hasPositions)
          {
            Vertex.Position.x = Positions[PolygonCount * 3 + 0];
            Vertex.Position.y = Positions[PolygonCount * 3 + 1];
            Vertex.Position.z = Positions[PolygonCount * 3 + 2];
          }

          if (hasNormals)
          {
            Vertex.Normal.x = Normals[PolygonCount * 3 + 0];
            Vertex.Normal.y = Normals[PolygonCount * 3 + 1];
            Vertex.Normal.z = Normals[PolygonCount * 3 + 2];
          }

          if (hasTexcoords)
          {
            Vertex.Texcoord.x = Texcoords[PolygonCount * 2 + 0];
            Vertex.Texcoord.y = Texcoords[PolygonCount * 2 + 1];
          }
        }
      }

      //  データの連結
      _pVertices->insert(_pVertices->end(), Vertices.begin(), Vertices.end());
    }

    //  インデックスデータの取得
    {
      vdl::uint IndicesNum;

      //  データの取得
      {
        const Microsoft::glTF::Accessor Accessor = _Document.accessors.Get(MeshPrimitive.indicesAccessorId);

        Indices Indices;
        if (Microsoft::glTF::ComponentType::COMPONENT_UNSIGNED_INT == Accessor.componentType)
        {
          Indices = _pResourceReader->ReadBinaryData<vdl::uint32_t>(_Document, Accessor);
        }
        else
        {
          for (auto& Index : _pResourceReader->ReadBinaryData<vdl::uint16_t>(_Document, Accessor))
          {
            Indices.push_back(static_cast<vdl::uint32_t>(Index));
          }
        }

        _pIndices->insert(_pIndices->end(), Indices.begin(), Indices.end());
        IndicesNum = static_cast<vdl::IndexType>(Indices.size());
      }

      //  データの状態を保存
      {
        Material& Material = (*_pMaterials)[MaterialCount];

        Material.IndexStart = Offset;
        Material.IndexCount = IndicesNum;
        Offset += IndicesNum;
      }
    }
  }
}
