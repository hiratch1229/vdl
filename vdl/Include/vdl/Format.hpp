#pragma once
#include "Fwd.hpp"

#include <string>
#include <assert.h>

namespace vdl
{
  enum class FormatType : uint8_t
  {
    eUnknown,
    eR32G32B32A32_Float,
    eR32G32B32A32_Uint,
    eR32G32B32A32_Sint,
    eR32G32B32_Float,
    eR32G32B32_Uint,
    eR32G32B32_Sint,
    eR16G16B16A16_Float,
    eR16G16B16A16_Unorm,
    eR16G16B16A16_Uint,
    eR16G16B16A16_Snorm,
    eR16G16B16A16_Sint,
    eR32G32_Float,
    eR32G32_Uint,
    eR32G32_Sint,
    eD32_Float_S8X24_Uint,
    eR10G10B10A2_Unorm,
    eR10G10B10A2_Uint,
    eR11G11B10_Float,
    eR8G8B8A8_Unorm,
    eR8G8B8A8_Unorm_SRGB,
    eR8G8B8A8_Uint,
    eR8G8B8A8_Snorm,
    eR8G8B8A8_Sint,
    eR16G16_Float,
    eR16G16_Unorm,
    eR16G16_Uint,
    eR16G16_Snorm,
    eR16G16_Sint,
    eD32_Float,
    eR32_Float,
    eR32_Uint,
    eR32_Sint,
    eD24_Unorm_S8_Uint,
    eR8G8_Unorm,
    eR8G8_Uint,
    eR8G8_Snorm,
    eR8G8_Sint,
    eR16_Float,
    eD16_Unorm,
    eR16_Unorm,
    eR16_Uint,
    eR16_Snorm,
    eR16_Sint,
    eR8_Unorm,
    eR8_Uint,
    eR8_Snorm,
    eR8_Sint,
    eR9G9B9E5_SharedExp,
    eR8G8_B8G8_Unorm,
    eG8R8_G8B8_Unorm,
    eBC1_Unorm,
    eBC1_Unorm_SRGB,
    eBC2_Unorm,
    eBC2_Unorm_SRGB,
    eBC3_Unorm,
    eBC3_Unorm_SRGB,
    eBC4_Unorm,
    eBC4_Snorm,
    eBC5_Unorm,
    eBC5_Snorm,
    eB5G6R5_Unorm,
    eB5G5R5A1_Unorm,
    eB8G8R8A8_Unorm,
    eB8G8R8X8_Unorm,
    eB8G8R8A8_Unorm_SRGB,
    eB8G8R8X8_Unorm_SRGB,
    eBC6H_UF16,
    eBC6H_SF16,
    eBC7_Unorm,
    eBC7_Unorm_SRGB,
    eB4G4R4A4_Unorm,

    eSwapChain = eR8G8B8A8_Unorm,
    eDepthStencil = eD16_Unorm,
  };
}

namespace std
{
  template <class Type>
  [[nodiscard]] inline string to_string(vdl::FormatType _Format)
  {
    switch (_Format)
    {
    case vdl::FormatType::eUnknown: return "Unknown";
    case vdl::FormatType::eR32G32B32A32_Float: return "R32G32B32A32_Float";
    case vdl::FormatType::eR32G32B32A32_Uint: return "R32G32B32A32_Uint";
    case vdl::FormatType::eR32G32B32A32_Sint: return "R32G32B32A32_Sint";
    case vdl::FormatType::eR32G32B32_Float: return "R32G32B32_Float";
    case vdl::FormatType::eR32G32B32_Uint: return "R32G32B32_Uint";
    case vdl::FormatType::eR32G32B32_Sint: return "R32G32B32_Sint";
    case vdl::FormatType::eR16G16B16A16_Float: return "R16G16B16A16_Float";
    case vdl::FormatType::eR16G16B16A16_Unorm: return "R16G16B16A16_Unorm";
    case vdl::FormatType::eR16G16B16A16_Uint: return "R16G16B16A16_Uint";
    case vdl::FormatType::eR16G16B16A16_Snorm: return "R16G16B16A16_Snorm";
    case vdl::FormatType::eR16G16B16A16_Sint: return "R16G16B16A16_Sint";
    case vdl::FormatType::eR32G32_Float: return "R32G32_Float";
    case vdl::FormatType::eR32G32_Uint: return "R32G32_Uint";
    case vdl::FormatType::eR32G32_Sint: return "R32G32_Sint";
    case vdl::FormatType::eD32_Float_S8X24_Uint: return "D32_Float_S8X24_Uint";
    case vdl::FormatType::eR10G10B10A2_Unorm: return "R10G10B10A2_Unorm";
    case vdl::FormatType::eR10G10B10A2_Uint: return "R10G10B10A2_Uint";
    case vdl::FormatType::eR11G11B10_Float: return "R11G11B10_Float";
    case vdl::FormatType::eR8G8B8A8_Unorm: return "R8G8B8A8_Unorm";
    case vdl::FormatType::eR8G8B8A8_Unorm_SRGB: return "R8G8B8A8_Unorm_SRGB";
    case vdl::FormatType::eR8G8B8A8_Uint: return "R8G8B8A8_Uint";
    case vdl::FormatType::eR8G8B8A8_Snorm: return "R8G8B8A8_Snorm";
    case vdl::FormatType::eR8G8B8A8_Sint: return "R8G8B8A8_Sint";
    case vdl::FormatType::eR16G16_Float: return "R16G16_Float";
    case vdl::FormatType::eR16G16_Unorm: return "R16G16_Unorm";
    case vdl::FormatType::eR16G16_Uint: return "R16G16_Uint";
    case vdl::FormatType::eR16G16_Snorm: return "R16G16_Snorm";
    case vdl::FormatType::eR16G16_Sint: return "R16G16_Sint";
    case vdl::FormatType::eD32_Float: return "D32_Float";
    case vdl::FormatType::eR32_Float: return "R32_Float";
    case vdl::FormatType::eR32_Uint: return "R32_Uint";
    case vdl::FormatType::eR32_Sint: return "R32_Sint";
    case vdl::FormatType::eD24_Unorm_S8_Uint: return "D24_Unorm_S8_Uint";
    case vdl::FormatType::eR8G8_Unorm: return "R8G8_Unorm";
    case vdl::FormatType::eR8G8_Uint: return "R8G8_Uint";
    case vdl::FormatType::eR8G8_Snorm: return "R8G8_Snorm";
    case vdl::FormatType::eR8G8_Sint: return "R8G8_Sint";
    case vdl::FormatType::eR16_Float: return "R16_Float";
    case vdl::FormatType::eD16_Unorm: return "D16_Unorm";
    case vdl::FormatType::eR16_Unorm: return "R16_Unorm";
    case vdl::FormatType::eR16_Uint: return "R16_Uint";
    case vdl::FormatType::eR16_Snorm: return "R16_Snorm";
    case vdl::FormatType::eR16_Sint: return "R16_Sint";
    case vdl::FormatType::eR8_Unorm: return "R8_Unorm";
    case vdl::FormatType::eR8_Uint: return "R8_Uint";
    case vdl::FormatType::eR8_Snorm: return "R8_Snorm";
    case vdl::FormatType::eR8_Sint: return "R8_Sint";
    case vdl::FormatType::eR9G9B9E5_SharedExp: return "R9G9B9E5_SharedExp";
    case vdl::FormatType::eR8G8_B8G8_Unorm: return "R8G8_B8G8_Unorm";
    case vdl::FormatType::eG8R8_G8B8_Unorm: return "G8R8_G8B8_Unorm";
    case vdl::FormatType::eBC1_Unorm: return "BC1_Unorm";
    case vdl::FormatType::eBC1_Unorm_SRGB: return "BC1_Unorm_SRGB";
    case vdl::FormatType::eBC2_Unorm: return "BC2_Unorm";
    case vdl::FormatType::eBC2_Unorm_SRGB: return "BC2_Unorm_SRGB";
    case vdl::FormatType::eBC3_Unorm: return "BC3_Unorm";
    case vdl::FormatType::eBC3_Unorm_SRGB: return "BC3_Unorm_SRGB";
    case vdl::FormatType::eBC4_Unorm: return "BC4_Unorm";
    case vdl::FormatType::eBC4_Snorm: return "BC4_Snorm";
    case vdl::FormatType::eBC5_Unorm: return "BC5_Unorm";
    case vdl::FormatType::eBC5_Snorm: return "BC5_Snorm";
    case vdl::FormatType::eB5G6R5_Unorm: return "B5G6R5_Unorm";
    case vdl::FormatType::eB5G5R5A1_Unorm: return "B5G5R5A1_Unorm";
    case vdl::FormatType::eB8G8R8A8_Unorm: return "B8G8R8A8_Unorm";
    case vdl::FormatType::eB8G8R8X8_Unorm: return "B8G8R8X8_Unorm";
    case vdl::FormatType::eB8G8R8A8_Unorm_SRGB: return "B8G8R8A8_Unorm_SRGB";
    case vdl::FormatType::eB8G8R8X8_Unorm_SRGB: return "B8G8R8X8_Unorm_SRGB";
    case vdl::FormatType::eBC6H_UF16: return "BC6H_UF16";
    case vdl::FormatType::eBC6H_SF16: return "BC6H_SF16";
    case vdl::FormatType::eBC7_Unorm: return "BC7_Unorm";
    case vdl::FormatType::eBC7_Unorm_SRGB: return "BC7_Unorm_SRGB";
    case vdl::FormatType::eB4G4R4A4_Unorm: return "B4G4R4A4_Unorm";
    default: return "Invalid";
    }
  }

  template <class Type>
  [[nodiscard]] inline wstring to_wstring(vdl::FormatType _Format)
  {
    switch (_Format)
    {
    case vdl::FormatType::eUnknown: return L"Unknown";
    case vdl::FormatType::eR32G32B32A32_Float: return L"R32G32B32A32_Float";
    case vdl::FormatType::eR32G32B32A32_Uint: return L"R32G32B32A32_Uint";
    case vdl::FormatType::eR32G32B32A32_Sint: return L"R32G32B32A32_Sint";
    case vdl::FormatType::eR32G32B32_Float: return L"R32G32B32_Float";
    case vdl::FormatType::eR32G32B32_Uint: return L"R32G32B32_Uint";
    case vdl::FormatType::eR32G32B32_Sint: return L"R32G32B32_Sint";
    case vdl::FormatType::eR16G16B16A16_Float: return L"R16G16B16A16_Float";
    case vdl::FormatType::eR16G16B16A16_Unorm: return L"R16G16B16A16_Unorm";
    case vdl::FormatType::eR16G16B16A16_Uint: return L"R16G16B16A16_Uint";
    case vdl::FormatType::eR16G16B16A16_Snorm: return L"R16G16B16A16_Snorm";
    case vdl::FormatType::eR16G16B16A16_Sint: return L"R16G16B16A16_Sint";
    case vdl::FormatType::eR32G32_Float: return L"R32G32_Float";
    case vdl::FormatType::eR32G32_Uint: return L"R32G32_Uint";
    case vdl::FormatType::eR32G32_Sint: return L"R32G32_Sint";
    case vdl::FormatType::eD32_Float_S8X24_Uint: return L"D32_Float_S8X24_Uint";
    case vdl::FormatType::eR10G10B10A2_Unorm: return L"R10G10B10A2_Unorm";
    case vdl::FormatType::eR10G10B10A2_Uint: return L"R10G10B10A2_Uint";
    case vdl::FormatType::eR11G11B10_Float: return L"R11G11B10_Float";
    case vdl::FormatType::eR8G8B8A8_Unorm: return L"R8G8B8A8_Unorm";
    case vdl::FormatType::eR8G8B8A8_Unorm_SRGB: return L"R8G8B8A8_Unorm_SRGB";
    case vdl::FormatType::eR8G8B8A8_Uint: return L"R8G8B8A8_Uint";
    case vdl::FormatType::eR8G8B8A8_Snorm: return L"R8G8B8A8_Snorm";
    case vdl::FormatType::eR8G8B8A8_Sint: return L"R8G8B8A8_Sint";
    case vdl::FormatType::eR16G16_Float: return L"R16G16_Float";
    case vdl::FormatType::eR16G16_Unorm: return L"R16G16_Unorm";
    case vdl::FormatType::eR16G16_Uint: return L"R16G16_Uint";
    case vdl::FormatType::eR16G16_Snorm: return L"R16G16_Snorm";
    case vdl::FormatType::eR16G16_Sint: return L"R16G16_Sint";
    case vdl::FormatType::eD32_Float: return L"D32_Float";
    case vdl::FormatType::eR32_Float: return L"R32_Float";
    case vdl::FormatType::eR32_Uint: return L"R32_Uint";
    case vdl::FormatType::eR32_Sint: return L"R32_Sint";
    case vdl::FormatType::eD24_Unorm_S8_Uint: return L"D24_Unorm_S8_Uint";
    case vdl::FormatType::eR8G8_Unorm: return L"R8G8_Unorm";
    case vdl::FormatType::eR8G8_Uint: return L"R8G8_Uint";
    case vdl::FormatType::eR8G8_Snorm: return L"R8G8_Snorm";
    case vdl::FormatType::eR8G8_Sint: return L"R8G8_Sint";
    case vdl::FormatType::eR16_Float: return L"R16_Float";
    case vdl::FormatType::eD16_Unorm: return L"D16_Unorm";
    case vdl::FormatType::eR16_Unorm: return L"R16_Unorm";
    case vdl::FormatType::eR16_Uint: return L"R16_Uint";
    case vdl::FormatType::eR16_Snorm: return L"R16_Snorm";
    case vdl::FormatType::eR16_Sint: return L"R16_Sint";
    case vdl::FormatType::eR8_Unorm: return L"R8_Unorm";
    case vdl::FormatType::eR8_Uint: return L"R8_Uint";
    case vdl::FormatType::eR8_Snorm: return L"R8_Snorm";
    case vdl::FormatType::eR8_Sint: return L"R8_Sint";
    case vdl::FormatType::eR9G9B9E5_SharedExp: return L"R9G9B9E5_SharedExp";
    case vdl::FormatType::eR8G8_B8G8_Unorm: return L"R8G8_B8G8_Unorm";
    case vdl::FormatType::eG8R8_G8B8_Unorm: return L"G8R8_G8B8_Unorm";
    case vdl::FormatType::eBC1_Unorm: return L"BC1_Unorm";
    case vdl::FormatType::eBC1_Unorm_SRGB: return L"BC1_Unorm_SRGB";
    case vdl::FormatType::eBC2_Unorm: return L"BC2_Unorm";
    case vdl::FormatType::eBC2_Unorm_SRGB: return L"BC2_Unorm_SRGB";
    case vdl::FormatType::eBC3_Unorm: return L"BC3_Unorm";
    case vdl::FormatType::eBC3_Unorm_SRGB: return L"BC3_Unorm_SRGB";
    case vdl::FormatType::eBC4_Unorm: return L"BC4_Unorm";
    case vdl::FormatType::eBC4_Snorm: return L"BC4_Snorm";
    case vdl::FormatType::eBC5_Unorm: return L"BC5_Unorm";
    case vdl::FormatType::eBC5_Snorm: return L"BC5_Snorm";
    case vdl::FormatType::eB5G6R5_Unorm: return L"B5G6R5_Unorm";
    case vdl::FormatType::eB5G5R5A1_Unorm: return L"B5G5R5A1_Unorm";
    case vdl::FormatType::eB8G8R8A8_Unorm: return L"B8G8R8A8_Unorm";
    case vdl::FormatType::eB8G8R8X8_Unorm: return L"B8G8R8X8_Unorm";
    case vdl::FormatType::eB8G8R8A8_Unorm_SRGB: return L"B8G8R8A8_Unorm_SRGB";
    case vdl::FormatType::eB8G8R8X8_Unorm_SRGB: return L"B8G8R8X8_Unorm_SRGB";
    case vdl::FormatType::eBC6H_UF16: return L"BC6H_UF16";
    case vdl::FormatType::eBC6H_SF16: return L"BC6H_SF16";
    case vdl::FormatType::eBC7_Unorm: return L"BC7_Unorm";
    case vdl::FormatType::eBC7_Unorm_SRGB: return L"BC7_Unorm_SRGB";
    case vdl::FormatType::eB4G4R4A4_Unorm: return L"B4G4R4A4_Unorm";
    default: return L"Invalid";
    }
  }
}
