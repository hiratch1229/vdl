#pragma once
#include "Type2.hpp"
#include "Type3.hpp"
#include "Types.hpp"
#include "Constants.hpp"

namespace vdl
{
  struct TextureVertex
  {
    float2 Position;
    float2 Texcoord;
  };
  
  struct StaticMeshVertex
  {
    float3 Position;
    float3 Normal;
    float3 Tangent;
    float2 Texcoord;
  };

  struct SkinnedMeshVertex : public StaticMeshVertex
  {
    float BoneWeights[Constants::kMaxBoneInfluence] = { 1.0f };
    uint BoneIndices[Constants::kMaxBoneInfluence] = {};
  };

  using IndexType = uint32_t;
}
