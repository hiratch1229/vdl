#pragma once
#include <vdl/Constants.hpp>

#include <vdl/Type2.hpp>
#include <vdl/Format.hpp>

namespace Constants
{
  constexpr vdl::uint kMaxBoneNum = vdl::Constants::kMaxBoneNum;

  constexpr vdl::uint kMaxBoneInfluence = vdl::Constants::kMaxBoneInfluence;

  constexpr vdl::uint kMaxController = vdl::Constants::kMaxController;

  constexpr vdl::uint kMaxCharacterNum = vdl::Constants::kMaxCharacterNum;

  constexpr vdl::uint kMaxSamplerNum = vdl::Constants::kMaxSamplerNum;

  constexpr vdl::uint kMaxTextureNum = vdl::Constants::kMaxTextureNum;

  constexpr vdl::uint kMaxConstantBufferNum = vdl::Constants::kMaxConstantBufferNum;

  constexpr const char* kDefaultTextureVertexShaderFilePath = vdl::Constants::kDefaultTextureVertexShaderFilePath;

  constexpr const char* kDefaultTexturePixelShaderFilePath = vdl::Constants::kDefaultTexturePixelShaderFilePath;

  constexpr const char* kDefaultStaticMeshVertexShaderFilePath = vdl::Constants::kDefaultStaticMeshVertexShaderFilePath;

  constexpr const char* kDefaultStaticMeshPixelShaderFilePath = vdl::Constants::kDefaultStaticMeshPixelShaderFilePath;

  constexpr const char* kDefaultSkinnedMeshVertexShaderFilePath = vdl::Constants::kDefaultSkinnedMeshVertexShaderFilePath;

  constexpr const char* kDefaultSkinnedMeshPixelShaderFilePath = vdl::Constants::kDefaultSkinnedMeshPixelShaderFilePath;

  constexpr vdl::uint kXInputButtonNum = 14;

  constexpr vdl::uint kMaxTextureBatchNum = 256;

  constexpr vdl::uint kMaxStaticMeshBatchNum = 128;

  constexpr vdl::uint kMaxSkinnedMeshBatchNum = 32;

  constexpr const char* kBinaryFileDirectory = "Bin/";

  constexpr const char* kBinaryFileFormat = ".bin";

  constexpr const char* kClassName = "vdl";

  constexpr const char* kDefaultWindowName = "vdl";

  constexpr vdl::uint2 kDefaultWindowSize = { 1280, 720 };

  constexpr vdl::uint kBackBufferNum = 3;

  constexpr vdl::Format kSwapChainFormat = vdl::Format::eR8G8B8A8_Unorm;

  constexpr vdl::Format kDepthStencilFormat = vdl::Format::eD24_Unorm_S8_Uint;
}
