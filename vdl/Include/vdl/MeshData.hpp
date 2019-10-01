#pragma once
#include "Types.hpp"
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
    Texture Diffuse;
    uint IndexStart = 0;
    uint IndexCount = 0;
  };

  struct StaticMeshData
  {
    std::string Name;
    std::vector<StaticMeshVertex> Vertices;
    std::vector<IndexType> Indices;
    std::vector<Material> Materials;
    Matrix GlobalTransform = Matrix::Identity();
  public:
    static StaticMeshData Rectangle(const Texture& _Diffuse = Palette::White);
    static StaticMeshData Box(const Texture& _Diffuse = Palette::White);
    static StaticMeshData Sphere(uint _SliceCount, uint _StackCount, const Texture& _Diffuse = Palette::White);
    //static StaticMeshData Capsule(uint _SliceCount, uint _StackCount, float _Height, const Texture& _Diffuse = Palette::White);
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

  struct SkinnedMeshData 
  {
    std::string Name;
    std::vector<SkinnedMeshVertex> Vertices;
    std::vector<IndexType> Indices;
    std::vector<Material> Materials;
    std::vector<Animation> Animations;
    Matrix GlobalTransform = Matrix::Identity();
  };
}
