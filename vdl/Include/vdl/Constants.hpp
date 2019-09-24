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

  constexpr const char* kDefaultTextureVertexShaderFilePath = "Shader/Texture/TextureVS.hlsl";

  constexpr const char* kDefaultTexturePixelShaderFilePath = "Shader/Texture/TexturePS.hlsl";

  constexpr const char* kDefaultStaticMeshVertexShaderFilePath = "Shader/StaticMesh/StaticMeshVS.hlsl";

  constexpr const char* kDefaultStaticMeshPixelShaderFilePath = "Shader/StaticMesh/StaticMeshPS.hlsl";

  constexpr const char* kDefaultSkinnedMeshVertexShaderFilePath = "";

  constexpr const char* kDefaultSkinnedMeshPixelShaderFilePath = "";
}
