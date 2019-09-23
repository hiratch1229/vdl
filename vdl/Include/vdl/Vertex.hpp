#pragma once
#include "Type2.hpp"
#include "Type3.hpp"
#include "Types.hpp"
#include "Constants.hpp"

namespace vdl
{
  struct Vertex2D
  {
    float2 Position;
    float2 Texcoord;
  };

  struct Vertex3D
  {
    float3 Position;
    float3 Normal;
    float3 Tangent;
    float2 Texcoord;

    float BoneWeights[Constants::kMaxBoneInfluence] = { 1.0f };
    uint BoneIndices[Constants::kMaxBoneInfluence] = {};
  };

  using IndexType = uint32_t;
}
