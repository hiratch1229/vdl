#pragma once
#include <vdl/ModelData.hpp>

#include <vdl/Buffer/Buffer.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct Mesh
{
  VertexBuffer VertexBuffer;
  IndexBuffer IndexBuffer;
  vdl::uint IndexStart;
  vdl::uint IndexCount;
  vdl::Material Material;
  vdl::Animations Animations;
  vdl::Matrix GlobalTransform;
};
using Meshes = std::vector<Mesh>;
