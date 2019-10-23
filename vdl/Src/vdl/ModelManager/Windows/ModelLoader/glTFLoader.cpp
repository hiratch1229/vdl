//#include "glTFLoader.hpp"
//
//#include <vdl/TextureManager/Windows/TextureLoader/TextureLoader.hpp>
//#include <vdl/Misc/Windows/Misc.hpp>
//
//#include <vdl/Macro.hpp>
//
//#include <filesystem>
//#include <sstream>
//
//namespace
//{
//  inline vdl::ColorF Cast(const Microsoft::glTF::Color4& _Color)
//  {
//    return { _Color.r, _Color.g, _Color.b, _Color.a };
//  }
//
//  inline vdl::Matrix Cast(const Microsoft::glTF::Matrix4& _Matrix)
//  {
//    return { _Matrix.values[0], _Matrix.values[1], _Matrix.values[2], _Matrix.values[3],
//     _Matrix.values[4], _Matrix.values[5], _Matrix.values[6], _Matrix.values[7],
//     _Matrix.values[8], _Matrix.values[9], _Matrix.values[10], _Matrix.values[11],
//     _Matrix.values[12], _Matrix.values[13], _Matrix.values[14], _Matrix.values[15] };
//  }
//
//  inline vdl::Matrix GetMatrixFromNode(const Microsoft::glTF::Node& _Node)
//  {
//    if (_Node.matrix == Microsoft::glTF::Matrix4::IDENTITY)
//    {
//      return vdl::Matrix::Scale({ _Node.scale.x, _Node.scale.y, _Node.scale.z })
//        * vdl::Matrix::Rotate({ _Node.rotation.x, _Node.rotation.y, _Node.rotation.z, _Node.rotation.w })
//        * vdl::Matrix::Translate({ _Node.translation.x, _Node.translation.y, _Node.translation.z });
//    }
//    else
//    {
//      return Cast(_Node.matrix);
//    }
//  }
//
//  class GLTFStreamReader : public Microsoft::glTF::IStreamReader
//  {
//  public:
//    GLTFStreamReader(std::filesystem::path _PathBase)
//      : PathBase_(std::move(_PathBase)) {}
//
//    std::shared_ptr<std::istream> GetInputStream(const std::string& _FileName)const override
//    {
//      const std::filesystem::path StreamPath = PathBase_ / std::filesystem::u8path(_FileName);
//      const std::shared_ptr<std::ifstream> IStream = std::make_shared<std::ifstream>(StreamPath, std::ios_base::binary);
//      assert(IStream && *IStream);
//
//      return IStream;
//    }
//  private:
//    std::filesystem::path PathBase_;
//  };
//}
//
//bool glTFLoader::CheckSupportFormat(const std::string& _FileFormat)
//{
//  return _FileFormat == "gltf" || _FileFormat == "glb" || _FileFormat == "vrm";
//}
//
////--------------------------------------------------
//
//MeshDatas glTFLoader::Load(const char* _FilePath)const
//{
//  const std::string FileFormat = GetFileFormat(_FilePath);
//  _ASSERT_EXPR_A(CheckSupportFormat(FileFormat), (FileFormat + "は対応していません。").c_str());
//
//  static constexpr vdl::Matrix Transform = vdl::Matrix(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
//
//  MeshDatas MeshDatas;
//
//  std::unique_ptr<Microsoft::glTF::GLTFResourceReader> pResourceReader;
//  const Microsoft::glTF::Document Document = LoadDocument(_FilePath, FileFormat, &pResourceReader);
//
//  const TextureLoader TextureLoader;
//  const std::string Directory = std::filesystem::path(_FilePath).remove_filename().string();
//
//  for (auto& GltfMesh : Document.meshes.Elements())
//  {
//    MeshData MeshData;
//    {
//      FetchMaterial(pResourceReader, Document, GltfMesh, Directory, &MeshData.Materials, TextureLoader);
//      FetchVerticesAndIndices(pResourceReader, Document, GltfMesh, &MeshData.Vertices, &MeshData.Indices, &MeshData.Materials);
//
//      MeshData.Name = GltfMesh.name;
//      MeshData.GlobalTransform = Transform * GetMatrixFromNode(Document.nodes.Get(GltfMesh.id));
//    }
//
//    MeshDatas.emplace_back(std::move(MeshData));
//  }
//
//  return MeshDatas;
//}
//
////--------------------------------------------------
//
//Microsoft::glTF::Document glTFLoader::LoadDocument(const char* _FilePath, const std::string& _FileFormat, std::unique_ptr<Microsoft::glTF::GLTFResourceReader>* _pResourceReader)const
//{
//  std::string JsonData;
//
//  const std::filesystem::path Path = _FilePath;
//
//  std::unique_ptr<GLTFStreamReader> StreamReader = std::make_unique<GLTFStreamReader>(Path.parent_path());
//  std::shared_ptr<std::istream> pIStream = StreamReader->GetInputStream(Path.filename().u8string());
//
//  if (_FileFormat == "gltf")
//  {
//    std::unique_ptr<Microsoft::glTF::GLTFResourceReader> pGltfResouceReader = std::make_unique<Microsoft::glTF::GLTFResourceReader>(std::move(StreamReader));
//    std::stringstream SStream;
//    SStream << pIStream->rdbuf();
//    JsonData = SStream.str();
//
//    *_pResourceReader = std::move(pGltfResouceReader);
//  }
//  else
//  {
//    std::unique_ptr<Microsoft::glTF::GLBResourceReader> pGlbResouceReader = std::make_unique<Microsoft::glTF::GLBResourceReader>(std::move(StreamReader), std::move(pIStream));
//    JsonData = pGlbResouceReader->GetJson();
//
//    *_pResourceReader = std::move(pGlbResouceReader);
//  }
//  assert(_pResourceReader);
//
//  return Microsoft::glTF::Deserialize(JsonData);
//}
//
//void glTFLoader::FetchMaterial(const std::unique_ptr<Microsoft::glTF::GLTFResourceReader>& _pResourceReader, const Microsoft::glTF::Document& _Document, const Microsoft::glTF::Mesh& _Mesh, const std::string& _Directory, Materials* _pMaterials, const TextureLoader& _TextureLoader)const
//{
//  assert(_pMaterials);
//
//  const vdl::uint MaterialNum = static_cast<vdl::uint>(_Mesh.primitives.size());
//  _pMaterials->resize(MaterialNum);
//
//  auto GetMaterialBinaryData = [&](vdl::CompressionImage* _pCompressionImage, const std::string& _TextureID)->void
//  {
//    if (!_TextureID.empty())
//    {
//      const Microsoft::glTF::Texture& Texture = _Document.textures.Get(_TextureID);
//      const Microsoft::glTF::Image& Image = _Document.images.Get(Texture.imageId);
//
//      if (!Image.bufferViewId.empty())
//      {
//        const Microsoft::glTF::BufferView& BufferView = _Document.bufferViews.Get(Image.bufferViewId);
//        (*_pCompressionImage) = _TextureLoader.LoadFromMemory(_pResourceReader->ReadBinaryData<vdl::uint8_t>(_Document, BufferView));
//      }
//      else
//      {
//        (*_pCompressionImage) = _TextureLoader.LoadFromFile((_Directory + Image.uri).c_str());
//      }
//    }
//  };
//
//  for (vdl::uint MaterialCount = 0; MaterialCount < MaterialNum; ++MaterialCount)
//  {
//    Material& Material = (*_pMaterials)[MaterialCount];
//
//    const Microsoft::glTF::Material& GltfMaterial = _Document.materials.Get(_Mesh.primitives[MaterialCount].materialId);
//
//    Material.MaterialColor = Cast(GltfMaterial.metallicRoughness.baseColorFactor);
//    GetMaterialBinaryData(&Material.Diffuse, GltfMaterial.metallicRoughness.baseColorTexture.textureId);
//    GetMaterialBinaryData(&Material.NormalMap, GltfMaterial.normalTexture.textureId);
//    GetMaterialBinaryData(&Material.MetallicRoughness, GltfMaterial.metallicRoughness.metallicRoughnessTexture.textureId);
//    GetMaterialBinaryData(&Material.Occlusion, GltfMaterial.occlusionTexture.textureId);
//    GetMaterialBinaryData(&Material.Emissive, GltfMaterial.emissiveTexture.textureId);
//  }
//}
//
//void glTFLoader::FetchVerticesAndIndices(const std::unique_ptr<Microsoft::glTF::GLTFResourceReader>& _pResourceReader, const Microsoft::glTF::Document& _Document, const Microsoft::glTF::Mesh& _Mesh, Vertices* _pVertices, Indices* _pIndices, Materials* _pMaterials)const
//{
//  assert(_pVertices && _pIndices && _pMaterials);
//
//  auto GetData = [&pResourceReader = _pResourceReader, Document = _Document](const Microsoft::glTF::MeshPrimitive& _MeshPrimitive, const std::string& _AccessorName)->std::vector<float>
//  {
//    std::string AccessorID;
//    if (!_MeshPrimitive.TryGetAttributeAccessorId(_AccessorName, AccessorID))
//    {
//      return {};
//    }
//
//    const Microsoft::glTF::Accessor Accessor = Document.accessors.Get(AccessorID);
//
//    return pResourceReader->ReadBinaryData<float>(Document, Accessor);
//  };
//
//  vdl::uint Offset = 0;
//
//  const vdl::uint MaterialNum = static_cast<vdl::uint>(_Mesh.primitives.size());
//  for (vdl::uint MaterialCount = 0; MaterialCount < MaterialNum; ++MaterialCount)
//  {
//    const Microsoft::glTF::MeshPrimitive& MeshPrimitive = _Mesh.primitives[MaterialCount];
//
//    //  頂点データの取得
//    {
//      Vertices Vertices;
//      {
//        const std::vector<float> Positions = GetData(MeshPrimitive, "POSITION");
//        const bool hasPositions = !Positions.empty();
//        const std::vector<float> Normals = GetData(MeshPrimitive, "NORMAL");
//        const bool hasNormals = !Normals.empty();
//        const std::vector<float> Texcoords = GetData(MeshPrimitive, "TEXCOORD_0");
//        const bool hasTexcoords = !Texcoords.empty();
//
//        const vdl::uint VertexNum = static_cast<vdl::uint>(Positions.size());
//        Vertices.resize(VertexNum);
//
//        const vdl::uint PolygonNum = VertexNum / 3;
//        for (vdl::uint PolygonCount = 0; PolygonCount < PolygonNum; ++PolygonCount)
//        {
//          vdl::Vertex3D& Vertex = Vertices[PolygonCount];
//
//          if (hasPositions)
//          {
//            Vertex.Position.x = Positions[PolygonCount * 3 + 0];
//            Vertex.Position.y = Positions[PolygonCount * 3 + 1];
//            Vertex.Position.z = Positions[PolygonCount * 3 + 2];
//          }
//
//          if (hasNormals)
//          {
//            Vertex.Normal.x = Normals[PolygonCount * 3 + 0];
//            Vertex.Normal.y = Normals[PolygonCount * 3 + 1];
//            Vertex.Normal.z = Normals[PolygonCount * 3 + 2];
//          }
//
//          if (hasTexcoords)
//          {
//            Vertex.Texcoord.x = Texcoords[PolygonCount * 2 + 0];
//            Vertex.Texcoord.y = Texcoords[PolygonCount * 2 + 1];
//          }
//        }
//      }
//
//      //  データの連結
//      _pVertices->insert(_pVertices->end(), Vertices.begin(), Vertices.end());
//    }
//
//    //  インデックスデータの取得
//    {
//      vdl::uint IndicesNum;
//
//      //  データの取得
//      {
//        const Microsoft::glTF::Accessor Accessor = _Document.accessors.Get(MeshPrimitive.indicesAccessorId);
//
//        Indices Indices;
//        {
//          if (Microsoft::glTF::ComponentType::COMPONENT_UNSIGNED_INT == Accessor.componentType)
//          {
//            Indices = _pResourceReader->ReadBinaryData<vdl::uint32_t>(_Document, Accessor);
//          }
//          else
//          {
//            for (auto& Index : _pResourceReader->ReadBinaryData<vdl::uint16_t>(_Document, Accessor))
//            {
//              Indices.push_back(static_cast<vdl::uint32_t>(Index));
//            }
//          }
//        }
//        IndicesNum = static_cast<vdl::IndexType>(Indices.size());
//        assert(IndicesNum % 3 == 0);
//
//        const vdl::uint PolygonNum = IndicesNum / 3;
//        for (vdl::uint PolygonCount = 0; PolygonCount < PolygonNum; ++PolygonCount)
//        {
//          vdl::Macro::Swap(Indices[PolygonCount * 3 + 0], Indices[PolygonCount * 3 + 2]);
//        }
//
//        _pIndices->insert(_pIndices->end(), Indices.begin(), Indices.end());
//      }
//
//      //  データの状態を保存
//      {
//        Material& Material = (*_pMaterials)[MaterialCount];
//
//        Material.IndexStart = Offset;
//        Material.IndexCount = IndicesNum;
//        Offset += IndicesNum;
//      }
//    }
//  }
//}

#include "glTFLoader.hpp"

#include <vdl/Constants/Constants.hpp>
#include <vdl/Misc/Misc.hpp>

#include <functional>

namespace
{
  inline vdl::Image Cast(const tinygltf::Image& _Image)
  {
    vdl::Image Image;
    {
      Image.Resize(vdl::uint2(_Image.width, _Image.height));

      //  RGBのみのフォーマット
      if (_Image.component == 3)
      {
        const vdl::uint8_t* glTFBuffer = _Image.image.data();
        vdl::Color* Buffer = Image.Buffer();

        const vdl::uint PixelNum = _Image.width * _Image.height;
        for (vdl::uint i = 0; i < PixelNum; ++i)
        {
          Buffer[i] = vdl::Color(glTFBuffer[i * 3 + 0], glTFBuffer[i * 3 + 1], glTFBuffer[i * 3 + 2]);
        }
      }
      //  RGBAのフォーマット
      else
      {
        ::memcpy(Image.Buffer(), _Image.image.data(), Image.BufferSize());
      }

    }

    return Image;
  }

  inline vdl::ColorF Cast(const tinygltf::ColorValue& _Color)
  {
    return { static_cast<float>(_Color[0]), static_cast<float>(_Color[1]), static_cast<float>(_Color[2]), static_cast<float>(_Color[3]) };
  }

  inline vdl::Matrix GetMatrixFromNode(const tinygltf::Node& _Node)
  {
    if (_Node.matrix.size() == 16)
    {
      return { static_cast<float>(_Node.matrix[0]), static_cast<float>(_Node.matrix[1]), static_cast<float>(_Node.matrix[2]), static_cast<float>(_Node.matrix[3]),
       static_cast<float>(_Node.matrix[4]), static_cast<float>(_Node.matrix[5]), static_cast<float>(_Node.matrix[6]), static_cast<float>(_Node.matrix[7]),
       static_cast<float>(_Node.matrix[8]), static_cast<float>(_Node.matrix[9]), static_cast<float>(_Node.matrix[10]), static_cast<float>(_Node.matrix[11]),
       static_cast<float>(_Node.matrix[12]), static_cast<float>(_Node.matrix[13]), static_cast<float>(_Node.matrix[14]), static_cast<float>(_Node.matrix[15]) };
    }
    else
    {
      return vdl::Matrix::Scale({ static_cast<float>(_Node.scale[0]), static_cast<float>(_Node.scale[1]), static_cast<float>(_Node.scale[2]) })
        * vdl::Matrix::Rotate({ static_cast<float>(_Node.rotation[0]), static_cast<float>(_Node.rotation[1]), static_cast<float>(_Node.rotation[2]), static_cast<float>(_Node.rotation[3]) })
        * vdl::Matrix::Translate({ static_cast<float>(_Node.translation[0]), static_cast<float>(_Node.translation[1]), static_cast<float>(_Node.translation[2]) });
    }
  }
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

  MeshDatas MeshDatas;

  tinygltf::Model Model;
  {
    tinygltf::TinyGLTF Loader;

    std::string Error;
    std::string Warning;

    bool Result = true;
    if (FileFormat == "gltf")
    {
      Result = Loader.LoadASCIIFromFile(&Model, &Error, &Warning, _FilePath);
    }
    else
    {
      Result = Loader.LoadBinaryFromFile(&Model, &Error, &Warning, _FilePath);
    }

    _ASSERT_EXPR_A(Result, Error.c_str());
  }

  Materials Materials;
  LoadMaterials(Model, &Materials);

  LoadMeshs(Model, &MeshDatas, Materials);

  return MeshDatas;
}

//--------------------------------------------------

void glTFLoader::LoadMaterials(const tinygltf::Model& _Model, Materials* _pMaterials)const
{
  assert(_pMaterials);

  for (auto& glTFMaterial : _Model.materials)
  {
    Material& Material = _pMaterials->emplace_back();

    auto LoadTexture = [&](vdl::CompressionImage* _pCompressionImage, const std::string& _Key)->bool
    {
      //  見つからなければ終了
      if (glTFMaterial.values.find(_Key) == glTFMaterial.values.end())
      {
        return false;
      }

      (*_pCompressionImage) = Cast(_Model.images[glTFMaterial.values.at(_Key).TextureIndex()]);

      return true;
    };
    auto LoadAddtinalTexture = [&](vdl::CompressionImage* _pCompressionImage, const std::string& _Key)->bool
    {
      //  見つからなければ終了
      if (glTFMaterial.additionalValues.find(_Key) == glTFMaterial.values.end())
      {
        return false;
      }

      (*_pCompressionImage) = Cast(_Model.images[glTFMaterial.additionalValues.at(_Key).TextureIndex()]);

      return true;
    };
    auto LoadExtensionlTexture = [&](vdl::CompressionImage* _pCompressionImage, const std::string& _Key)->bool
    {
      //  見つからなければ終了
      if (glTFMaterial.extensions.find("KHR_materials_pbrSpecularGlossiness") == glTFMaterial.extensions.end())
      {
        return false;
      }

      const auto& Extension = glTFMaterial.extensions.find("KHR_materials_pbrSpecularGlossiness");
      //  見つからなければ終了
      if (!Extension->second.Has(_Key))
      {
        return false;
      }

      (*_pCompressionImage) = Cast(_Model.images[Extension->second.Get(_Key).Get("index").Get<int>()]);

      return true;
    };

    if (LoadTexture(&Material.Diffuse, "baseColorTexture"))
    {
      Material.MaterialColor = Cast(glTFMaterial.values.at("baseColorTexture").ColorFactor());
    }
    LoadExtensionlTexture(&Material.Specular, "specularGlossinessTexture");
    LoadAddtinalTexture(&Material.NormalMap, "normalTexture");
    LoadTexture(&Material.MetallicRoughness, "metallicRoughnessTexture");
    LoadAddtinalTexture(&Material.Emissive, "emissiveTexture");
  }
}

void glTFLoader::LoadMeshs(const tinygltf::Model& _Model, MeshDatas* _pMeshDatas, const Materials& _Materials)const
{
  assert(_pMeshDatas);

  const tinygltf::Scene& Scene = _Model.scenes[_Model.defaultScene > -1 ? _Model.defaultScene : 0];

  std::function<void(const tinygltf::Node&, const vdl::Matrix&)> LoadNode =
    [&](const tinygltf::Node& _Node, const vdl::Matrix& _BaseMatrix)
  {
    //  親の行列に自分の行列を掛ける
    const vdl::Matrix GlobalTransform = _BaseMatrix * GetMatrixFromNode(_Node);

    //  子のノードを探索
    for (auto& ChildrenNodeIndex : _Node.children)
    {
      LoadNode(_Model.nodes[ChildrenNodeIndex], GlobalTransform);
    }

    //  メッシュデータが存在しないなら終了
    if (_Node.mesh < 0)
    {
      return;
    }

    MeshData MeshData;
    {
      const tinygltf::Mesh& Mesh = _Model.meshes[_Node.mesh];

      MeshData.Name = Mesh.name;
      MeshData.GlobalTransform = GlobalTransform;
      for (auto& Primitive : Mesh.primitives)
      {
        //  頂点データの取得
        {
          Vertices Vertices;

          auto GetPrimitiveData = [&](const float* _pBuffer, const std::string& _Key)->size_t
          {
            if (Primitive.attributes.find(_Key) != Primitive.attributes.end())
            {
              _pBuffer = nullptr;
              return 0;
            }

            const tinygltf::Accessor& Accessor = _Model.accessors[Primitive.attributes.at(_Key)];
            const tinygltf::BufferView& BufferView = _Model.bufferViews[Accessor.bufferView];

            _pBuffer = reinterpret_cast<const float*>(&_Model.buffers[BufferView.buffer].data[Accessor.byteOffset + BufferView.byteOffset]);
            return Accessor.count;
          };

          const float* PosBuffer = nullptr;
          const size_t VertexNum = GetPrimitiveData(PosBuffer, "POSITION");
          assert(VertexNum > 0);

          const float* NormalBuffer = nullptr;
          GetPrimitiveData(NormalBuffer, "NORMAL");

          const float* TexcoordBuffer = nullptr;
          GetPrimitiveData(TexcoordBuffer, "TEXCOORD_0");

          const float* JointBuffer = nullptr;
          GetPrimitiveData(JointBuffer, "JOINTS_0");

          const float* WeightBuffer = nullptr;
          GetPrimitiveData(WeightBuffer, "WEIGHTS_0");

          Vertices.resize(VertexNum);
          for (size_t VertexCount = 0; VertexCount < VertexNum; ++VertexCount)
          {
            vdl::Vertex3D& Vertex = Vertices[VertexCount];
            {
              Vertex.Position = vdl::float3(PosBuffer[VertexCount * 3 + 0], PosBuffer[VertexCount * 3 + 1], PosBuffer[VertexCount * 3 + 2]);
              Vertex.Normal = NormalBuffer ? vdl::float3(NormalBuffer[VertexCount * 3 + 0], NormalBuffer[VertexCount * 3 + 1], NormalBuffer[VertexCount * 3 + 2]) : vdl::float3::Forward();
              Vertex.Tangent = Vertex.Normal.Cross(vdl::float3::Up());
              Vertex.Texcoord = TexcoordBuffer ? vdl::float2(TexcoordBuffer[VertexCount * 2 + 0], TexcoordBuffer[VertexCount * 2 + 1]) : vdl::float2(0.0);

              //  ボーンが入ってるとき
              if (JointBuffer && WeightBuffer)
              {
                constexpr vdl::uint kBoneInfluence = (Constants::kMaxBoneInfluence < 4 ? Constants::kMaxBoneInfluence : 4);
                for (vdl::uint i = 0; i < 4; ++i)
                {
                  Vertex.BoneIndices[i] = static_cast<vdl::uint>(JointBuffer[VertexCount * 4 + i]);
                  Vertex.BoneWeights[i] = WeightBuffer[VertexCount * 4 + i];
                }
              }
            }
          }

          MeshData.Vertices.insert(MeshData.Vertices.cend(), Vertices.begin(), Vertices.end());
        }

        //  インデックスデータの取得
        {
          Indices Indices;
          assert(Primitive.indices > -1);

          const tinygltf::Accessor& Accessor = _Model.accessors[Primitive.indices];
          const tinygltf::BufferView& BufferView = _Model.bufferViews[Accessor.bufferView];

          const void* IndexBuffer = &_Model.buffers[BufferView.buffer];
          const size_t IndexNum = Accessor.count;
          assert(IndexNum % 3 == 0);

          Indices.resize(IndexNum);
          switch (Accessor.componentType)
          {
          case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
            for (size_t IndexCount = 0; IndexCount < IndexNum; ++IndexCount)
            {
              Indices[IndexCount] = static_cast<const vdl::uint32_t*>(IndexBuffer)[IndexCount];
            }
            break;
          case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
            for (size_t IndexCount = 0; IndexCount < IndexNum; ++IndexCount)
            {
              Indices[IndexCount] = static_cast<const vdl::uint16_t*>(IndexBuffer)[IndexCount];
            }
            break;
          case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
            for (size_t IndexCount = 0; IndexCount < IndexNum; ++IndexCount)
            {
              Indices[IndexCount] = static_cast<const vdl::uint8_t*>(IndexBuffer)[IndexCount];
            }
            break;
          default: assert(false);
          }

          const size_t PolygonNum = IndexNum / 3;
          for (size_t PolygonCount = 0; PolygonCount < PolygonNum; ++PolygonCount)
          {
            vdl::Macro::Swap(Indices[PolygonCount * 3 + 0], Indices[PolygonCount * 3 + 2]);
          }

          Material Material = _Materials[Primitive.material];
          {
            Material.IndexStart = static_cast<vdl::uint>(MeshData.Indices.size());
            Material.IndexCount = static_cast<vdl::uint>(IndexNum);
          }
          MeshData.Materials.emplace_back(Material);

          MeshData.Indices.insert(MeshData.Indices.cend(), Indices.begin(), Indices.end());
        }
      }
    }
  };

  constexpr vdl::Matrix kInitGlobalTransform = vdl::Matrix(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
  for (auto& NodeIndex : Scene.nodes)
  {
    LoadNode(_Model.nodes[NodeIndex], kInitGlobalTransform);
  }
}
