#pragma once

namespace vdl
{
  enum class Format
  {
    eUnknown,
    eR32G32B32A32_Typeless,
    eR32G32B32A32_Float,
    eR32G32B32A32_Uint,
    eR32G32B32A32_Sint,
    eR32G32B32_Typeless,
    eR32G32B32_Float,
    eR32G32B32_Uint,
    eR32G32B32_Sint,
    eR16G16B16A16_Typeless,
    eR16G16B16A16_Float,
    eR16G16B16A16_Unorm,
    eR16G16B16A16_Uint,
    eR16G16B16A16_Snorm,
    eR16G16B16A16_Sint,
    eR32G32_Typeless,
    eR32G32_Float,
    eR32G32_Uint,
    eR32G32_Sint,
    eR32G8X24_Typeless,
    eD32_Float_S8X24_Uint,
    eR32_Float_X8X24_Typeless,
    eX32_Typeless_G8X24_Uint,
    eR10G10B10A2_Typeless,
    eR10G10B10A2_Unorm,
    eR10G10B10A2_Uint,
    eR11G11B10_Float,
    eR8G8B8A8_Typeless,
    eR8G8B8A8_Unorm,
    eR8G8B8A8_Unorm_SRGB,
    eR8G8B8A8_Uint,
    eR8G8B8A8_Snorm,
    eR8G8B8A8_Sint,
    eR16G16_Typeless,
    eR16G16_Float,
    eR16G16_Unorm,
    eR16G16_Uint,
    eR16G16_Snorm,
    eR16G16_Sint,
    eR32_Typeless,
    eD32_Float,
    eR32_Float,
    eR32_Uint,
    eR32_Sint,
    eR24G8_Typeless,
    eD24_Unorm_S8_Uint,
    eR24_Unorm_X8_Typeless,
    eX24_Typeless_G8_Uint,
    eR8G8_Typeless,
    eR8G8_Unorm,
    eR8G8_Uint,
    eR8G8_Snorm,
    eR8G8_Sint,
    eR16_Typeless,
    eR16_Float,
    eD16_Unorm,
    eR16_Unorm,
    eR16_Uint,
    eR16_Snorm,
    eR16_Sint,
    eR8_Typeless,
    eR8_Unorm,
    eR8_Uint,
    eR8_Snorm,
    eR8_Sint,
    eA8_Unorm,
    eR1_Unorm,
    eR9G9B9E5_SharedExp,
    eR8G8_B8G8_Unorm,
    eG8R8_G8B8_Unorm,
    eBC1_Typeless,
    eBC1_Unorm,
    eBC1_Unorm_SRGB,
    eBC2_Typeless,
    eBC2_Unorm,
    eBC2_Unorm_SRGB,
    eBC3_Typeless,
    eBC3_Unorm,
    eBC3_Unorm_SRGB,
    eBC4_Typeless,
    eBC4_Unorm,
    eBC4_Snorm,
    eBC5_Typeless,
    eBC5_Unorm,
    eBC5_Snorm,
    eB5G6R5_Unorm,
    eB5G5R5A1_Unorm,
    eB8G8R8A8_Unorm,
    eB8G8R8X8_Unorm,
    eR10G10B10_XR_BIAS_A2_Unorm,
    eB8G8R8A8_Typeless,
    eB8G8R8A8_Unorm_SRGB,
    eB8G8R8X8_Typeless,
    eB8G8R8X8_Unorm_SRGB,
    eBC6H_Typeless,
    eBC6H_UF16,
    eBC6H_SF16,
    eBC7_Typeless,
    eBC7_Unorm,
    eBC7_Unorm_SRGB,
    eB4G4R4A4_Unorm,

    eSwapChain = eR8G8B8A8_Unorm,
    eDepthStencil = eD24_Unorm_S8_Uint,
  };
}
