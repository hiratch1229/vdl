#pragma once
#include <vdl/Matrix.hpp>
#include <vdl/Color.hpp>
#include <vdl/Types.hpp>
#include <vdl/Texture.hpp>

#include <vdl/Buffer/IBuffer.hpp>

#include <string>
#include <unordered_map>
#include <memory>

struct Mesh
{
  struct Material
  {
    struct Property
    {
      vdl::Texture Texture;
      vdl::ColorF Color;
    };
    Property Diffuse;
    vdl::uint IndexStart = 0;
    vdl::uint IndexCount = 0;
  };
  struct Animation
  {
    struct Skeletal
    {
      struct Bone
      {
        vdl::Matrix Offset;
        vdl::Matrix Pose;
      };
      std::unordered_map<std::string, Bone> Bones;
    };
    std::vector<Skeletal> Skeletals;
  };

  std::unique_ptr<IBuffer> VertexBuffer;
  std::unique_ptr<IBuffer> IndexBuffer;
  std::vector<Material> Materials;
  std::vector<Animation> Animations;
  vdl::Matrix GlobalTransform = vdl::Matrix::Identity();
};
