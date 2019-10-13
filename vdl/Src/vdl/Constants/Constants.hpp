#pragma once
#include <vdl/Constants.hpp>

#include <vdl/Type2.hpp>
#include <vdl/Color.hpp>
#include <vdl/Macro.hpp>
#include <vdl/Format.hpp>

namespace Constants
{
  constexpr vdl::uint2 kDefaultWindowSize = vdl::Constants::kDefaultWindowSize;

  constexpr vdl::uint kDefaultMaxFPS = vdl::Constants::kDefaultMaxFPS;

  constexpr vdl::uint kMaxBoneNum = vdl::Constants::kMaxBoneNum;

  constexpr vdl::uint kMaxBoneInfluence = vdl::Constants::kMaxBoneInfluence;

  constexpr vdl::uint kMaxController = vdl::Constants::kMaxController;

  constexpr vdl::uint kMaxCharacterNum = vdl::Constants::kMaxCharacterNum;

  constexpr vdl::uint kMaxRenderTextureNum = vdl::Constants::kMaxRenderTextureNum;

  constexpr vdl::uint kMaxSamplerNum = vdl::Constants::kMaxSamplerNum;

  constexpr vdl::uint kMaxShaderResourceNum = vdl::Constants::kMaxShaderResourceNum;

  constexpr vdl::uint kMaxConstantBufferNum = vdl::Constants::kMaxConstantBufferNum;

  constexpr vdl::uint kMaxUnorderedAccessObjectNum = vdl::Constants::kMaxUnorderedAccessObjectNum;

  constexpr const char* kDefaultTextureVertexShaderCode = vdl::Constants::kDefaultTextureVertexShaderCode;

  constexpr vdl::uint kDefaultTextureVertexShaderSize = static_cast<vdl::uint>(vdl::Macro::ArraySize(vdl::Constants::kDefaultTextureVertexShaderCode));

  constexpr const char* kDefaultTexturePixelShaderCode = vdl::Constants::kDefaultTexturePixelShaderCode;

  constexpr vdl::uint kDefaultTexturePixelShaderSize = static_cast<vdl::uint>(vdl::Macro::ArraySize(vdl::Constants::kDefaultTexturePixelShaderCode));

  constexpr const char* kDefaultMeshVertexShaderCode = vdl::Constants::kDefaultMeshVertexShaderCode;

  constexpr vdl::uint kDefaultMeshVertexShaderSize = static_cast<vdl::uint>(vdl::Macro::ArraySize(vdl::Constants::kDefaultMeshVertexShaderCode));

  constexpr const char* kDefaultMeshPixelShaderCode = vdl::Constants::kDefaultMeshPixelShaderCode;

  constexpr vdl::uint kDefaultMeshPixelShaderSize = static_cast<vdl::uint>(vdl::Macro::ArraySize(vdl::Constants::kDefaultMeshPixelShaderCode));

  constexpr vdl::uint kAnimationSamplingRate = 24;

  constexpr float kAnimationSamplingTime = 1.0f / kAnimationSamplingRate;

  constexpr vdl::uint kXInputButtonNum = 14;

  constexpr vdl::uint kMaxTextureBatchNum = 256;

  constexpr vdl::uint kMaxMeshBatchNum = 128;

  constexpr const char* kBinaryFileDirectory = "Bin/";

  constexpr const char* kBinaryFileFormat = ".bin";

  constexpr const char* kClassName = "vdl";

  constexpr const char* kDefaultWindowName = "vdl";

  constexpr vdl::ColorF kDefaultScreenClearColor = vdl::Palette::Black;

  constexpr float kDefaultClearDepth = 1.0f;

  constexpr vdl::uint kDefaultClearStencil = 0;

  constexpr vdl::uint kBackBufferNum = 3;

  constexpr vdl::FormatType kTextureFormat = vdl::FormatType::eR8G8B8A8_Unorm;
}
