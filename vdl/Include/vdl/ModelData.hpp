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
    uint IndexStart = 0;
    uint IndexCount;
    Material Material;
    //Animations Animations;
    Matrix GlobalTransform = Matrix::Identity();
  };

  struct ModelData
  {
    Vertices Vertices;
    Indices Indices;
    MeshDatas MeshDatas;
  public:
    static ModelData Rectangle(const Texture& _Diffuse = Palette::White, const Texture& _NormalMap = Color(128, 128, 255));
    static ModelData Box(const Texture& _Diffuse = Palette::White, const Texture& _NormalMap = Color(128, 128, 255));
    static ModelData Sphere(uint _SliceCount, uint _StackCount, const Texture& _Diffuse = Palette::White, const Texture& _NormalMap = Color(128, 128, 255));
    //static ModelData Capsule(uint _SliceCount, uint _StackCount, float _Height, const Texture& _Diffuse = Palette::White);
  };
}
