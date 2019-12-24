#pragma once
#include "Fwd.hpp"
#include "Type2.hpp"
#include "Type3.hpp"
#include "Types.hpp"
#include "Matrix.hpp"
#include "Constants.hpp"
#include <array>

namespace vdl
{
  struct Vertex2D
  {
    float2 Position;
    float2 Texcoord;
  public:
    [[nodiscard]] bool operator==(const Vertex2D& _Vertex)const { return Position == _Vertex.Position && Texcoord == _Vertex.Texcoord; }
    [[nodiscard]] bool operator!=(const Vertex2D& _Vertex)const { return Position != _Vertex.Position || Texcoord != _Vertex.Texcoord; }
  };

  struct VertexStaticMesh
  {
    float3 Position;
    float3 Normal;
    float3 Tangent;
    float2 Texcoord;
  public:
    [[nodiscard]] bool operator==(const VertexStaticMesh& _Vertex)const { return Position == _Vertex.Position && Normal == _Vertex.Normal && Tangent == _Vertex.Tangent && Texcoord == _Vertex.Texcoord; }
    [[nodiscard]] bool operator!=(const VertexStaticMesh& _Vertex)const { return Position != _Vertex.Position || Normal != _Vertex.Normal || Tangent != _Vertex.Tangent || Texcoord != _Vertex.Texcoord; }
  };

  struct VertexSkinnedMesh : public VertexStaticMesh
  {
    std::array<float, Constants::kMaxBoneInfluence> BoneWeights = { 1.0f };
    std::array<uint, Constants::kMaxBoneInfluence> BoneIndices = {};
  public:
    VertexSkinnedMesh() = default;

    VertexSkinnedMesh(const VertexStaticMesh& _VertexStaticMesh)
      : VertexStaticMesh(_VertexStaticMesh) {}
  public:
    [[nodiscard]] bool operator==(const VertexSkinnedMesh& _Vertex)const { return Position == _Vertex.Position && Normal == _Vertex.Normal && Tangent == _Vertex.Tangent && Texcoord == _Vertex.Texcoord && BoneWeights == _Vertex.BoneWeights && BoneIndices == _Vertex.BoneIndices; }
    [[nodiscard]] bool operator!=(const VertexSkinnedMesh& _Vertex)const { return Position != _Vertex.Position || Normal != _Vertex.Normal || Tangent != _Vertex.Tangent || Texcoord != _Vertex.Texcoord || BoneWeights != _Vertex.BoneWeights || BoneIndices != _Vertex.BoneIndices; }
  };

  struct Instance2D
  {
    Matrix NDCTransform;
    float2 TexcoordScale;
    float2 TexcoordTranslate;
    Color4F Color;
  };

  struct InstanceStaticMesh
  {
    Matrix World;
    Color4F Color;
  };

  struct InstanceSkinnedMesh : public InstanceStaticMesh
  {
    //  TODO:SkinnedMesh
  };
}
