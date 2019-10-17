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
    ColorF MaterialColor;
    Texture Diffuse;
    Texture NormalMap;
    uint IndexNum;
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

  struct MeshData
  {
    std::string Name;
    std::vector<Vertex3D> Vertices;
    std::vector<IndexType> Indices;
    std::vector<Animation> Animations;
    Material Material;
    Matrix GlobalTransform = Matrix::Identity();
  public:
    static MeshData Rectangle(const Texture& _Diffuse = Palette::White, const Texture& _NormalMap = Color(128, 128, 255));
    static MeshData Box(const Texture& _Diffuse = Palette::White, const Texture& _NormalMap = Color(128, 128, 255));
    static MeshData Sphere(uint _SliceCount, uint _StackCount, const Texture& _Diffuse = Palette::White, const Texture& _NormalMap = Color(128, 128, 255));
    //static MeshData Capsule(uint _SliceCount, uint _StackCount, float _Height, const Texture& _Diffuse = Palette::White);
  };
}
