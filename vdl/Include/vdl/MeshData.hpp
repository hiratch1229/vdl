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
    Texture Diffuse;
    ColorF MaterialColor;
    uint IndexStart = 0;
    uint IndexCount = 0;
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
    std::vector<Material> Materials;
    std::vector<Animation> Animations;
    Matrix GlobalTransform = Matrix::Identity();
  public:
    static MeshData Rectangle(const Texture& _Diffuse = Palette::White);
    static MeshData Box(const Texture& _Diffuse = Palette::White);
    static MeshData Sphere(uint _SliceCount, uint _StackCount, const Texture& _Diffuse = Palette::White);
    //static MeshData Capsule(uint _SliceCount, uint _StackCount, float _Height, const Texture& _Diffuse = Palette::White);
  };
}
