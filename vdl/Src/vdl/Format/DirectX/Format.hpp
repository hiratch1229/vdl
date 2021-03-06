#pragma once
#include <vdl/Format.hpp>

#include <dxgi.h>
#include <assert.h>

inline constexpr DXGI_FORMAT Cast(vdl::FormatType _Format)
{
  switch (_Format)
  {
  case vdl::FormatType::eUnknown:
    return DXGI_FORMAT_UNKNOWN;
  case vdl::FormatType::eR32G32B32A32_Float:
    return DXGI_FORMAT_R32G32B32A32_FLOAT;
  case vdl::FormatType::eR32G32B32A32_Uint:
    return DXGI_FORMAT_R32G32B32A32_UINT;
  case vdl::FormatType::eR32G32B32A32_Sint:
    return DXGI_FORMAT_R32G32B32A32_SINT;
  case vdl::FormatType::eR16G16B16A16_Float:
    return DXGI_FORMAT_R16G16B16A16_FLOAT;
  case vdl::FormatType::eR16G16B16A16_Unorm:
    return DXGI_FORMAT_R16G16B16A16_UNORM;
  case vdl::FormatType::eR16G16B16A16_Uint:
    return DXGI_FORMAT_R16G16B16A16_UINT;
  case vdl::FormatType::eR16G16B16A16_Snorm:
    return DXGI_FORMAT_R16G16B16A16_SNORM;
  case vdl::FormatType::eR16G16B16A16_Sint:
    return DXGI_FORMAT_R16G16B16A16_SINT;
  case vdl::FormatType::eR32G32_Float:
    return DXGI_FORMAT_R32G32_FLOAT;
  case vdl::FormatType::eR32G32_Uint:
    return DXGI_FORMAT_R32G32_UINT;
  case vdl::FormatType::eR32G32_Sint:
    return DXGI_FORMAT_R32G32_SINT;
  case vdl::FormatType::eD32_Float_S8X24_Uint:
    return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
  case vdl::FormatType::eR10G10B10A2_Unorm:
    return DXGI_FORMAT_R10G10B10A2_UNORM;
  case vdl::FormatType::eR10G10B10A2_Uint:
    return DXGI_FORMAT_R10G10B10A2_UINT;
  case vdl::FormatType::eR11G11B10_Float:
    return DXGI_FORMAT_R11G11B10_FLOAT;
  case vdl::FormatType::eR8G8B8A8_Unorm:
    return DXGI_FORMAT_R8G8B8A8_UNORM;
  case vdl::FormatType::eR8G8B8A8_Unorm_SRGB:
    return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
  case vdl::FormatType::eR8G8B8A8_Uint:
    return DXGI_FORMAT_R8G8B8A8_UINT;
  case vdl::FormatType::eR8G8B8A8_Snorm:
    return DXGI_FORMAT_R8G8B8A8_SNORM;
  case vdl::FormatType::eR8G8B8A8_Sint:
    return DXGI_FORMAT_R8G8B8A8_SINT;
  case vdl::FormatType::eR16G16_Float:
    return DXGI_FORMAT_R16G16_FLOAT;
  case vdl::FormatType::eR16G16_Unorm:
    return DXGI_FORMAT_R16G16_UNORM;
  case vdl::FormatType::eR16G16_Uint:
    return DXGI_FORMAT_R16G16_UINT;
  case vdl::FormatType::eR16G16_Snorm:
    return DXGI_FORMAT_R16G16_SNORM;
  case vdl::FormatType::eR16G16_Sint:
    return DXGI_FORMAT_R16G16_SINT;
  case vdl::FormatType::eD32_Float:
    return DXGI_FORMAT_D32_FLOAT;
  case vdl::FormatType::eR32_Float:
    return DXGI_FORMAT_R32_FLOAT;
  case vdl::FormatType::eR32_Uint:
    return DXGI_FORMAT_R32_UINT;
  case vdl::FormatType::eR32_Sint:
    return DXGI_FORMAT_R32_SINT;
  case vdl::FormatType::eD24_Unorm_S8_Uint:
    return DXGI_FORMAT_D24_UNORM_S8_UINT;
  case vdl::FormatType::eR8G8_Unorm:
    return DXGI_FORMAT_R8G8_UNORM;
  case vdl::FormatType::eR8G8_Uint:
    return DXGI_FORMAT_R8G8_UINT;
  case vdl::FormatType::eR8G8_Snorm:
    return DXGI_FORMAT_R8G8_SNORM;
  case vdl::FormatType::eR8G8_Sint:
    return DXGI_FORMAT_R8G8_SINT;
  case vdl::FormatType::eR16_Float:
    return DXGI_FORMAT_R16_FLOAT;
  case vdl::FormatType::eD16_Unorm:
    return DXGI_FORMAT_D16_UNORM;
  case vdl::FormatType::eR16_Unorm:
    return DXGI_FORMAT_R16_UNORM;
  case vdl::FormatType::eR16_Uint:
    return DXGI_FORMAT_R16_UINT;
  case vdl::FormatType::eR16_Snorm:
    return DXGI_FORMAT_R16_SNORM;
  case vdl::FormatType::eR16_Sint:
    return DXGI_FORMAT_R16_SINT;
  case vdl::FormatType::eR8_Unorm:
    return DXGI_FORMAT_R8_UNORM;
  case vdl::FormatType::eR8_Uint:
    return DXGI_FORMAT_R8_UINT;
  case vdl::FormatType::eR8_Snorm:
    return DXGI_FORMAT_R8_SNORM;
  case vdl::FormatType::eR8_Sint:
    return DXGI_FORMAT_R8_SINT;
  case vdl::FormatType::eR9G9B9E5_SharedExp:
    return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
  case vdl::FormatType::eR8G8_B8G8_Unorm:
    return DXGI_FORMAT_R8G8_B8G8_UNORM;
  case vdl::FormatType::eG8R8_G8B8_Unorm:
    return DXGI_FORMAT_G8R8_G8B8_UNORM;
  case vdl::FormatType::eBC1_Unorm:
    return DXGI_FORMAT_BC1_UNORM;
  case vdl::FormatType::eBC1_Unorm_SRGB:
    return DXGI_FORMAT_BC1_UNORM_SRGB;
  case vdl::FormatType::eBC2_Unorm:
    return DXGI_FORMAT_BC2_UNORM;
  case vdl::FormatType::eBC2_Unorm_SRGB:
    return DXGI_FORMAT_BC2_UNORM_SRGB;
  case vdl::FormatType::eBC3_Unorm:
    return DXGI_FORMAT_BC3_UNORM;
  case vdl::FormatType::eBC3_Unorm_SRGB:
    return DXGI_FORMAT_BC3_UNORM_SRGB;
  case vdl::FormatType::eBC4_Unorm:
    return DXGI_FORMAT_BC4_UNORM;
  case vdl::FormatType::eBC4_Snorm:
    return DXGI_FORMAT_BC4_SNORM;
  case vdl::FormatType::eBC5_Unorm:
    return DXGI_FORMAT_BC5_UNORM;
  case vdl::FormatType::eBC5_Snorm:
    return DXGI_FORMAT_BC5_SNORM;
  case vdl::FormatType::eB5G6R5_Unorm:
    return DXGI_FORMAT_B5G6R5_UNORM;
  case vdl::FormatType::eB5G5R5A1_Unorm:
    return DXGI_FORMAT_B5G5R5A1_UNORM;
  case vdl::FormatType::eB8G8R8A8_Unorm:
    return DXGI_FORMAT_B8G8R8A8_UNORM;
  case vdl::FormatType::eB8G8R8X8_Unorm:
    return DXGI_FORMAT_B8G8R8X8_UNORM;
  case vdl::FormatType::eB8G8R8A8_Unorm_SRGB:
    return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
  case vdl::FormatType::eB8G8R8X8_Unorm_SRGB:
    return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
  case vdl::FormatType::eBC6H_UF16:
    return DXGI_FORMAT_BC6H_UF16;
  case vdl::FormatType::eBC6H_SF16:
    return DXGI_FORMAT_BC6H_SF16;
  case vdl::FormatType::eBC7_Unorm:
    return DXGI_FORMAT_BC7_UNORM;
  case vdl::FormatType::eBC7_Unorm_SRGB:
    return DXGI_FORMAT_BC7_UNORM_SRGB;
  case vdl::FormatType::eB4G4R4A4_Unorm:
    return DXGI_FORMAT_B4G4R4A4_UNORM;
  default: assert(false);
  }

  return DXGI_FORMAT_UNKNOWN;
}

inline constexpr DXGI_FORMAT TextureFormatFromDepthStencilFormat(vdl::FormatType _Format)
{
  switch (_Format)
  {
  case vdl::FormatType::eD32_Float_S8X24_Uint:
    return DXGI_FORMAT_R32G8X24_TYPELESS;
  case vdl::FormatType::eD32_Float:
    return DXGI_FORMAT_R32_TYPELESS;
  case vdl::FormatType::eD24_Unorm_S8_Uint:
    return DXGI_FORMAT_R24G8_TYPELESS;
  case vdl::FormatType::eD16_Unorm:
    return DXGI_FORMAT_R16_TYPELESS;
  default: assert(false);
  }

  return DXGI_FORMAT_UNKNOWN;
}

inline constexpr DXGI_FORMAT DepthFormatFromDepthStencilFormat(vdl::FormatType _Format)
{
  switch (_Format)
  {
  case vdl::FormatType::eD32_Float_S8X24_Uint:
    return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
  case vdl::FormatType::eD32_Float:
    return DXGI_FORMAT_R32_FLOAT;
  case vdl::FormatType::eD24_Unorm_S8_Uint:
    return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
  case vdl::FormatType::eD16_Unorm:
    return DXGI_FORMAT_R16_UNORM;
  default: assert(false);
  }

  return DXGI_FORMAT_UNKNOWN;
}

inline constexpr DXGI_FORMAT StencilFormatFromDepthStencilFormat(vdl::FormatType _Format)
{
  switch (_Format)
  {
  case vdl::FormatType::eD32_Float_S8X24_Uint:
    return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
  case vdl::FormatType::eD24_Unorm_S8_Uint:
    return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
  default: assert(false);
  }

  return DXGI_FORMAT_UNKNOWN;
}
