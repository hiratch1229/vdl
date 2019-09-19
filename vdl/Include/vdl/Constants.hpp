#pragma once
#include "Types.hpp"

namespace vdl::Constants
{
  constexpr uint kMaxBoneNum = 32;

  constexpr uint kMaxBoneInfluence = 4;

  constexpr uint kMaxController = 4;

  constexpr uint kMaxCharacterNum = 256;

  constexpr uint kMaxSamplerNum = 16;

  constexpr uint kMaxTextureNum = 128;

  constexpr uint kMaxConstantBufferNum = 14;

  constexpr const char* kDefaultTextureVertexShaderFilePath = "";

  constexpr const char* kDefaultTexturePixelShaderFilePath = "";

  constexpr const char* kDefaultStaticMeshVertexShaderFilePath = "";

  constexpr const char* kDefaultStaticMeshPixelShaderFilePath = "";

  constexpr const char* kDefaultSkinnedMeshVertexShaderFilePath = "";

  constexpr const char* kDefaultSkinnedMeshPixelShaderFilePath = "";
}
