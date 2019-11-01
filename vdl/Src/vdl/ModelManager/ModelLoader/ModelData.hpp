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

  CEREAL_SERIALIZE(MaterialColor, Diffuse, Specular, NormalMap, MetallicRoughness, Emissive)
};
using Materials = std::vector<Material>;

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
using Animations = std::vector<Animation>;

struct MeshData
{
  vdl::uint IndexStart;
  vdl::uint IndexCount = 0;
  vdl::uint MaterialIndex = 0;
  vdl::Matrix GlobalTransform;
  //Animations Animations;

  CEREAL_SERIALIZE(IndexStart, IndexCount, MaterialIndex, GlobalTransform /*,Animations*/)
};
using MeshDatas = std::vector<MeshData>;

struct ModelData
{
  vdl::Vertices Vertices;
  vdl::Indices Indices;
  Materials Materials;
  MeshDatas MeshDatas;

  CEREAL_SERIALIZE(Vertices, Indices, Materials, MeshDatas)
};
