#pragma once
#include <vdl/Format.hpp>

#include <vdl/pch/Vulkan/pch.hpp>

inline constexpr vk::Format Cast(vdl::FormatType _Format)
{
  switch (_Format)
  {
  case vdl::FormatType::eUnknown:
    return vk::Format::eUndefined;
  case vdl::FormatType::eR32G32B32A32_Float:
    return vk::Format::eR32G32B32A32Sfloat;
  case vdl::FormatType::eR32G32B32A32_Uint:
    return vk::Format::eR32G32B32A32Uint;
  case vdl::FormatType::eR32G32B32A32_Sint:
    return vk::Format::eR32G32B32A32Sint;
  case vdl::FormatType::eR32G32B32_Float:
    return vk::Format::eR32G32B32Sfloat;
  case vdl::FormatType::eR32G32B32_Uint:
    return vk::Format::eR32G32B32Uint;
  case vdl::FormatType::eR32G32B32_Sint:
    return vk::Format::eR32G32B32Sint;
  case vdl::FormatType::eR16G16B16A16_Float:
    return vk::Format::eR16G16B16A16Sfloat;
  case vdl::FormatType::eR16G16B16A16_Unorm:
    return vk::Format::eR16G16B16A16Unorm;
  case vdl::FormatType::eR16G16B16A16_Uint:
    return vk::Format::eR16G16B16A16Uint;
  case vdl::FormatType::eR16G16B16A16_Snorm:
    return vk::Format::eR16G16B16A16Snorm;
  case vdl::FormatType::eR16G16B16A16_Sint:
    return vk::Format::eR16G16B16A16Sint;
  case vdl::FormatType::eR32G32_Float:
    return vk::Format::eR32G32Sfloat;
  case vdl::FormatType::eR32G32_Uint:
    return vk::Format::eR32G32Uint;
  case vdl::FormatType::eR32G32_Sint:
    return vk::Format::eR32G32Sint;
  case vdl::FormatType::eD32_Float_S8X24_Uint:
    return vk::Format::eD32SfloatS8Uint;
  case vdl::FormatType::eR10G10B10A2_Unorm:
    return vk::Format::eA2R10G10B10UnormPack32;
  case vdl::FormatType::eR10G10B10A2_Uint:
    return vk::Format::eA2R10G10B10UintPack32;
  case vdl::FormatType::eR11G11B10_Float:
    return vk::Format::eB10G11R11UfloatPack32;
  case vdl::FormatType::eR8G8B8A8_Unorm:
    return vk::Format::eR8G8B8A8Unorm;
  case vdl::FormatType::eR8G8B8A8_Unorm_SRGB:
    return vk::Format::eR8G8B8A8Srgb;
  case vdl::FormatType::eR8G8B8A8_Uint:
    return vk::Format::eR8G8B8A8Uint;
  case vdl::FormatType::eR8G8B8A8_Snorm:
    return vk::Format::eR8G8B8A8Snorm;
  case vdl::FormatType::eR8G8B8A8_Sint:
    return vk::Format::eR8G8B8A8Sint;
  case vdl::FormatType::eR16G16_Float:
    return vk::Format::eR16G16Sfloat;
  case vdl::FormatType::eR16G16_Unorm:
    return vk::Format::eR16G16Unorm;
  case vdl::FormatType::eR16G16_Uint:
    return vk::Format::eR16G16Uint;
  case vdl::FormatType::eR16G16_Snorm:
    return vk::Format::eR16G16Snorm;
  case vdl::FormatType::eR16G16_Sint:
    return vk::Format::eR16G16Sint;
  case vdl::FormatType::eD32_Float:
    return vk::Format::eD32Sfloat;
  case vdl::FormatType::eR32_Float:
    return vk::Format::eR32Sfloat;
  case vdl::FormatType::eR32_Uint:
    return vk::Format::eR32Uint;
  case vdl::FormatType::eR32_Sint:
    return vk::Format::eR32Sint;
  case vdl::FormatType::eD24_Unorm_S8_Uint:
    return vk::Format::eD24UnormS8Uint;
  case vdl::FormatType::eR8G8_Unorm:
    return vk::Format::eR8G8Unorm;
  case vdl::FormatType::eR8G8_Uint:
    return vk::Format::eR8G8Uint;
  case vdl::FormatType::eR8G8_Snorm:
    return vk::Format::eR8G8Snorm;
  case vdl::FormatType::eR8G8_Sint:
    return vk::Format::eR8G8Sint;
  case vdl::FormatType::eR16_Float:
    return vk::Format::eR16Sfloat;
  case vdl::FormatType::eD16_Unorm:
    return vk::Format::eD16Unorm;
  case vdl::FormatType::eR16_Unorm:
    return vk::Format::eR16Unorm;
  case vdl::FormatType::eR16_Uint:
    return vk::Format::eR16Uint;
  case vdl::FormatType::eR16_Snorm:
    return vk::Format::eR16Uint;
  case vdl::FormatType::eR16_Sint:
    return vk::Format::eR16Sint;
  case vdl::FormatType::eR8_Unorm:
    return vk::Format::eR8Unorm;
  case vdl::FormatType::eR8_Uint:
    return vk::Format::eR8Uint;
  case vdl::FormatType::eR8_Snorm:
    return vk::Format::eR8Snorm;
  case vdl::FormatType::eR8_Sint:
    return vk::Format::eR8Sint;
  case vdl::FormatType::eR9G9B9E5_SharedExp:
    return vk::Format::eE5B9G9R9UfloatPack32;
  case vdl::FormatType::eR8G8_B8G8_Unorm:
    return vk::Format::eB8G8R8G8422Unorm;
  case vdl::FormatType::eG8R8_G8B8_Unorm:
    return vk::Format::eG8B8G8R8422Unorm;
  case vdl::FormatType::eBC1_Unorm:
    return vk::Format::eBc1RgbaUnormBlock;
  case vdl::FormatType::eBC1_Unorm_SRGB:
    return vk::Format::eBc1RgbaSrgbBlock;
  case vdl::FormatType::eBC2_Unorm:
    return vk::Format::eBc2UnormBlock;
  case vdl::FormatType::eBC2_Unorm_SRGB:
    return vk::Format::eBc2UnormBlock;
  case vdl::FormatType::eBC3_Unorm:
    return vk::Format::eBc3UnormBlock;
  case vdl::FormatType::eBC3_Unorm_SRGB:
    return vk::Format::eBc3SrgbBlock;
  case vdl::FormatType::eBC4_Unorm:
    return vk::Format::eBc4UnormBlock;
  case vdl::FormatType::eBC4_Snorm:
    return vk::Format::eBc4SnormBlock;
  case vdl::FormatType::eBC5_Unorm:
    return vk::Format::eBc5UnormBlock;
  case vdl::FormatType::eBC5_Snorm:
    return vk::Format::eBc5SnormBlock;
  case vdl::FormatType::eB5G6R5_Unorm:
    return vk::Format::eB5G6R5UnormPack16;
  case vdl::FormatType::eB5G5R5A1_Unorm:
    return vk::Format::eB5G5R5A1UnormPack16;
  case vdl::FormatType::eB8G8R8A8_Unorm:
    return vk::Format::eB8G8R8A8Unorm;
  case vdl::FormatType::eB8G8R8X8_Unorm:
    return vk::Format::eB8G8R8Unorm;
  case vdl::FormatType::eB8G8R8A8_Unorm_SRGB:
    return vk::Format::eB8G8R8A8Srgb;
  case vdl::FormatType::eB8G8R8X8_Unorm_SRGB:
    return vk::Format::eB8G8R8Srgb;
  case vdl::FormatType::eBC6H_UF16:
    return vk::Format::eBc6HUfloatBlock;
  case vdl::FormatType::eBC6H_SF16:
    return vk::Format::eBc6HSfloatBlock;
  case vdl::FormatType::eBC7_Unorm:
    return vk::Format::eBc7UnormBlock;
  case vdl::FormatType::eBC7_Unorm_SRGB:
    return vk::Format::eBc7SrgbBlock;
  case vdl::FormatType::eB4G4R4A4_Unorm:
    return vk::Format::eB5G5R5A1UnormPack16;
  default: assert(false);
  }

  return vk::Format::eUndefined;
}

inline bool ContainsStencil(vk::Format _Format)
{
  constexpr vk::Format kStencilFormats[] = {
    vk::Format::eD16UnormS8Uint,
    vk::Format::eD24UnormS8Uint,
    vk::Format::eD32SfloatS8Uint
  };

  for (auto& StencilFormat : kStencilFormats)
  {
    if (_Format == StencilFormat)
    {
      return true;
    }
  }

  return false;
}
