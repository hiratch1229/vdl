#pragma once
#include <vdl/Constants.hpp>

#include <vdl/Format.hpp>

namespace Constants
{
  constexpr vdl::uint kMaxBoneInfluence = vdl::Constants::kMaxBoneInfluence;

  constexpr vdl::uint kMaxController = vdl::Constants::kMaxController;

  constexpr vdl::uint kMaxCharacterNum = vdl::Constants::kMaxCharacterNum;

  constexpr vdl::uint kXInputButtonNum = 14;

  constexpr const char* kClassName = "vdl";

  constexpr const char* kDefaultWindowName = "vdl";

  constexpr vdl::uint2 kDefaultWindowSize = { 1280, 720 };

  constexpr vdl::uint kBackBufferNum = 3;

  constexpr vdl::Format kSwapChainFormat = vdl::Format::eR8G8B8A8_Unorm;

  constexpr vdl::Format kDepthStencilFormat = vdl::Format::eD24_Unorm_S8_Uint;

  constexpr const char* kBinaryFileDirectory = "Bin/";

  constexpr const char* kBinaryFileFormat = ".bin";
}
