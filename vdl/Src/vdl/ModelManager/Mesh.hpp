#pragma once
#include <vdl/MeshData.hpp>

#include <vdl/Buffer/Buffer.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct Mesh
{
  std::string Name;
  VertexBuffer VertexBuffer;
  IndexBuffer IndexBuffer;
  std::vector<vdl::Animation> Animations;
  vdl::Material Material;
  vdl::Matrix GlobalTransform;
};
using Meshes = std::vector<Mesh>;
