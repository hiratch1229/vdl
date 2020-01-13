#pragma once
#include <vdl/Flag.hpp>

enum class PlatformFlag : vdl::uint8_t
{
  eDirectX11 = 1 << 0,
  eDirectX12 = 1 << 1,
  eVulkan = 1 << 2,

  eWindows = 1 << 3,
  eLinux = 1 << 4,

  eCommon = static_cast<uint8_t>(~0),
};
using PlatformFlags = vdl::Flags<PlatformFlag, vdl::uint8_t>;
