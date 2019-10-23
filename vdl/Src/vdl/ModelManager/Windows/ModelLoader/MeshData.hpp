#pragma once
#include <vdl/Image.hpp>
#include <vdl/Color.hpp>
#include <vdl/Matrix.hpp>
#include <vdl/Vertex.hpp>

#include <vdl/Serialize.hpp>

struct Material
{
  vdl::ColorF MaterialColor = vdl::Palette::White;
  vdl::CompressionImage Diffuse;
  vdl::CompressionImage Specular;
  vdl::CompressionImage NormalMap;
  vdl::CompressionImage MetallicRoughness;
  vdl::CompressionImage Emissive;
  vdl::uint IndexStart = 0;
  vdl::uint IndexCount = 0;

  CEREAL_SERIALIZE(MaterialColor, Diffuse, Specular, NormalMap, MetallicRoughness, Emissive, IndexStart, IndexCount)
};

struct Bone
{
  std::string Name;
  vdl::Matrix Offset;
  vdl::Matrix Pose;

  CEREAL_SERIALIZE(Name, Offset, Pose)
};
using Skeletal = std::vector<Bone>;

struct Animation
{
  std::string Name;
  std::vector<Skeletal> Skeletals;

  CEREAL_SERIALIZE(Name, Skeletals)
};

using Vertices = std::vector<vdl::Vertex3D>;
using Indices = std::vector<vdl::IndexType>;
using Materials = std::vector<Material>;
using Animations = std::vector<Animation>;

struct MeshData
{
  std::string Name;
  Vertices Vertices;
  Indices Indices;
  Materials Materials;
  Animations Animations;
  vdl::Matrix GlobalTransform;

  CEREAL_SERIALIZE(Name, Vertices, Indices, Materials, Animations, GlobalTransform)
};
using MeshDatas = std::vector<MeshData>;
