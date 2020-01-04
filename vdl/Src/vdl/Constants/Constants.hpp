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

  constexpr const char* kDefaultTextureShaderCode = vdl::Constants::kDefaultTextureShaderCode;

  constexpr vdl::uint kDefaultTextureShaderSize = static_cast<vdl::uint>(vdl::Macro::ArraySize(vdl::Constants::kDefaultTextureShaderCode));

  constexpr const char* kDefaultStaticMeshShaderCode = vdl::Constants::kDefaultStaticMeshShaderCode;

  constexpr vdl::uint kDefaultStaticMeshShaderSize = static_cast<vdl::uint>(vdl::Macro::ArraySize(vdl::Constants::kDefaultStaticMeshShaderCode));

  constexpr const char* kDefaultSkinnedMeshShaderCode = vdl::Constants::kDefaultSkinnedMeshShaderCode;

  constexpr vdl::uint kDefaultSkinnedMeshShaderSize = static_cast<vdl::uint>(vdl::Macro::ArraySize(vdl::Constants::kDefaultSkinnedMeshShaderCode));

  constexpr const char* kDefaultShaderEntryPointVS = vdl::Constants::kDefaultShaderEntryPointVS;

  constexpr const char* kDefaultShaderEntryPointPS = vdl::Constants::kDefaultShaderEntryPointPS;

  constexpr vdl::uint kAnimationSamplingRate = 24;

  constexpr float kAnimationSamplingTime = 1.0f / kAnimationSamplingRate;

  constexpr vdl::uint kXInputButtonNum = 14;

  constexpr vdl::uint kMaxTextureBatchNum = 256;

  constexpr vdl::uint kMaxStaticMeshBatchNum = 256;

  constexpr vdl::uint kMaxSkinnedMeshBatchNum = 128;

  constexpr vdl::uint kMaxSkyboxBatchNum = 8;

  constexpr const char* kBinaryFileDirectory = "Bin/";

  constexpr const char* kBinaryFileFormat = ".bin";

#if defined VDL_USING_DIRECTX11
  constexpr const char* kShaderBinaryFileFormat = ".d3d11bin";
#elif defined VDL_USING_DIRECTX12
  constexpr const char* kShaderBinaryFileFormat = ".d3d12bin";
#elif defined VDL_USING_VULKAN
  constexpr const char* kShaderBinaryFileFormat = ".vkbin";
#endif

  constexpr const char* kScreenShotFileDirectory = "ScreenShot/";

  constexpr const char* kClassName = "vdl";

  constexpr const char* kDefaultWindowName = "vdl";

  constexpr vdl::Color4F kDefaultScreenClearColor = vdl::Palette::Black;

  constexpr float kDefaultClearDepth = 1.0f;

  constexpr vdl::uint kDefaultClearStencil = 0;

  constexpr vdl::uint kBackBufferNum = 3;

  constexpr vdl::FormatType kTextureFormat = vdl::FormatType::eR8G8B8A8_Unorm;

  //  低グラフィック描画API用の定数
#if defined VDL_USING_DIRECTX12 | VDL_USING_VULKAN
  constexpr vdl::uint kInstanceBufferSizeMultiple = 5;

  constexpr vdl::uint kGraphicsCommandBufferNum = 5;

  constexpr vdl::uint kComputeCommandBufferNum = 3;

  constexpr vdl::uint kParentConstantBufferSize = 51200;

  constexpr vdl::uint kConstantBufferAlignment = 256;
#endif
}
