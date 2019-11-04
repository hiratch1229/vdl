#pragma once
#include <vdl/Format.hpp>

inline constexpr bool hasStencil(vdl::FormatType _Format)
{
  return _Format == vdl::FormatType::eD24_Unorm_S8_Uint || _Format == vdl::FormatType::eD32_Float_S8X24_Uint;
}
