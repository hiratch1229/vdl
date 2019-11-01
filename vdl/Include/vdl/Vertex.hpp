#pragma once
#include "Fwd.hpp"
#include "Type2.hpp"
#include "Type3.hpp"
#include "Types.hpp"
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

  struct Vertex3D
  {
    float3 Position;
    float3 Normal;
    float3 Tangent;
    float2 Texcoord;
    std::array<float, Constants::kMaxBoneInfluence> BoneWeights = { 1.0f };
    std::array<uint, Constants::kMaxBoneInfluence> BoneIndices = {};
  public:
    [[nodiscard]] bool operator==(const Vertex3D& _Vertex)const { return Position == _Vertex.Position && Normal == _Vertex.Normal && Tangent == _Vertex.Tangent && Texcoord == _Vertex.Texcoord && BoneWeights == _Vertex.BoneWeights && BoneIndices == _Vertex.BoneIndices; }
    [[nodiscard]] bool operator!=(const Vertex3D& _Vertex)const { return Position != _Vertex.Position || Normal != _Vertex.Normal || Tangent != _Vertex.Tangent || Texcoord != _Vertex.Texcoord || BoneWeights != _Vertex.BoneWeights || BoneIndices != _Vertex.BoneIndices; }
  };
}
