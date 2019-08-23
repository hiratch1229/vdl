#pragma once
#include "Matrix.hpp"
#include "Color.hpp"
#include "Types.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"

#include <vector>
#include <unordered_map>
#include <string>

namespace vdl
{
  struct StaticMeshData
  {
    struct Material
    {
      struct Property
      {
        Texture Texture;
        ColorF Color;
      };
      Property Diffuse;
      uint IndexStart = 0;
      uint IndexCount = 0;
    };

    std::vector<Vertex3D> Vertices;
    std::vector<IndexType> Indices;
    std::vector<Material> Materials;
    Matrix GlobalTransform = Matrix::Identity();
  };

  struct SkinnedMeshData : public StaticMeshData
  {
    struct Animation
    {
      struct Skeletal
      {
        struct Bone
        {
          Matrix Offset;
          Matrix Pose;
        };
        std::unordered_map<std::string, Bone> Bones;
      };
      std::vector<Skeletal> Skeletals;
    };

    std::vector<Animation> Animations;
  };
}
