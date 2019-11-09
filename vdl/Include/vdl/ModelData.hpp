#pragma once
#include "Fwd.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "Matrix.hpp"

#include <vector>
#include <unordered_map>
#include <string>

namespace vdl
{
  struct Material
  {
    ColorF MaterialColor = Palette::White;
    Texture Diffuse;
    Texture NormalMap;
  };

  struct StaticMeshData
  {
    uint IndexStart = 0;
    uint IndexCount;
    Material Material;
    Matrix GlobalTransform = Matrix::Identity();
  };

  struct StaticModelData
  {
    VertexStaticMeshs Vertices;
    Indices Indices;
    StaticMeshDatas MeshDatas;
  public:
    static StaticModelData Rectangle(const Texture& _Diffuse = Palette::Diffuse, const Texture& _NormalMap = Palette::NormalMap);
    static StaticModelData Box(const Texture& _Diffuse = Palette::Diffuse, const Texture& _NormalMap = Palette::NormalMap);
    static StaticModelData Sphere(uint _SliceCount, uint _StackCount, const Texture& _Diffuse = Palette::Diffuse, const Texture& _NormalMap = Palette::NormalMap);
    //static StaticModelData Capsule(uint _SliceCount, uint _StackCount, float _Height, const Texture& _Diffuse = Palette::Diffuse);
  };

  struct Bone
  {
    Matrix Offset;
    Matrix Pose;
  };

  struct Skeletal
  {
    std::unordered_map<std::string, Bone> Bones;
  };

  struct Animation
  {
    std::vector<Skeletal> Skeletals;
  };

  struct SkinnedMeshData : public StaticMeshData
  {
    Animations Animations;
  public:
    SkinnedMeshData() = default;

    SkinnedMeshData(const StaticMeshData& _StaticMeshData)
      : StaticMeshData(_StaticMeshData) {}

    SkinnedMeshData(StaticMeshData&& _StaticMeshData)
      : StaticMeshData(_StaticMeshData) {}
  };

  struct SkinnedModelData
  {
    VertexSkinnedMeshs Vertices;
    Indices Indices;
    SkinnedMeshDatas MeshDatas;
  public:
    SkinnedModelData() = default;

    SkinnedModelData(const StaticModelData& _StaticModelData)
      : Indices(_StaticModelData.Indices)
    {
      const size_t VertexNum = _StaticModelData.Vertices.size();
      Vertices.resize(VertexNum);

      for (size_t VertexCount = 0; VertexCount < VertexNum; ++VertexCount)
      {
        Vertices[VertexCount] = _StaticModelData.Vertices[VertexCount];
      }

      const size_t MeshNum = _StaticModelData.MeshDatas.size();
      MeshDatas.resize(MeshNum);

      for (size_t MeshCount = 0; MeshCount < MeshNum; ++MeshCount)
      {
        MeshDatas[MeshCount] = _StaticModelData.MeshDatas[MeshCount];
      }
    }

    SkinnedModelData(StaticModelData&& _StaticModelData)
      : Indices(std::move(_StaticModelData.Indices))
    {
      const size_t VertexNum = _StaticModelData.Vertices.size();
      Vertices.resize(VertexNum);

      for (size_t VertexCount = 0; VertexCount < VertexNum; ++VertexCount)
      {
        Vertices[VertexCount] = std::move(_StaticModelData.Vertices[VertexCount]);
      }

      const size_t MeshNum = _StaticModelData.MeshDatas.size();
      MeshDatas.resize(MeshNum);

      for (size_t MeshCount = 0; MeshCount < MeshNum; ++MeshCount)
      {
        MeshDatas[MeshCount] = std::move(_StaticModelData.MeshDatas[MeshCount]);
      }
    }
  };
}
