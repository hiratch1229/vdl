#pragma once
#include "Types.hpp"

namespace vdl::Constants
{
  constexpr uint kMaxBoneNum = 32;

  constexpr uint kMaxBoneInfluence = 4;

  constexpr uint kMaxController = 4;

  constexpr uint kMaxCharacterNum = 256;

  constexpr uint kMaxRenderTextureNum = 8;

  constexpr uint kMaxSamplerNum = 16;

  constexpr uint kMaxTextureNum = 128;

  constexpr uint kMaxConstantBufferNum = 14;

  constexpr const char* kDefaultTextureVertexShaderFilePath = "vdl/Shader/Texture/TextureVS.hlsl";

  constexpr const char* kDefaultTexturePixelShaderFilePath = "vdl/Shader/Texture/TexturePS.hlsl";

  constexpr const char* kDefaultStaticMeshVertexShaderFilePath = "vdl/Shader/StaticMesh/StaticMeshVS.hlsl";

  constexpr const char* kDefaultStaticMeshPixelShaderFilePath = "vdl/Shader/StaticMesh/StaticMeshPS.hlsl";

  constexpr const char* kDefaultSkinnedMeshVertexShaderFilePath = "vdl/Shader/SkinnedMesh/SkinnedMeshVS.hlsl";

  constexpr const char* kDefaultSkinnedMeshPixelShaderFilePath = "vdl/Shader/SkinnedMesh/SkinnedMeshPS.hlsl";
}
