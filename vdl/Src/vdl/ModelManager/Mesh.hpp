#pragma once
#include <vdl/MeshData.hpp>

#include <vdl/Buffer/IBuffer.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct Mesh
{
  std::string Name;
  std::unique_ptr<IBuffer> pVertexBuffer;
  std::unique_ptr<IBuffer> pIndexBuffer;
  std::vector<vdl::Material> Materials;
  std::vector<vdl::Animation> Animations;
  vdl::Matrix GlobalTransform;
};
using Meshes = std::vector<Mesh>;
