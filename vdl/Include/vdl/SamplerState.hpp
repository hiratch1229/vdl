#pragma once
#include "Types.hpp"
#include "Color.hpp"

namespace vdl
{
  enum class Filter : uint8_t
  {
    MinMagMipPoint = 0,
    MinMagPointMipLinear = 0x1,
    MinPointMagLinearMipPoint = 0x4,
    MinPointMagMipLinear = 0x5,
    MinLinearMagMipPoint = 0x10,
    MinLinearMagPointMipLinear = 0x11,
    MinMagLinearMipPoint = 0x14,
    MinMagMipLinear = 0x15,
    Anisotropic = 0x55,
    ComparisonMinMagMipPoint = 0x80,
    ComparisonMinMagPointMipLinear = 0x81,
    ComparisonMinPointMagLinearMipPoint = 0x84,
    ComparisonMinPointMagMipLinear = 0x85,
    ComparisonMinLinearMagMipPoint = 0x90,
    ComparisonMinLinearMagPointMipLinear = 0x91,
    ComparisonMinMagLinearMipPoint = 0x94,
    ComparisonMinMagMipLinear = 0x95,
    ComparisonAnisotropic = 0xd5,
  };

  enum class AddressMode : uint8_t
  {
    Wrap = 1,
    Mirror = 2,
    Clamp = 3,
    Border = 4,
    MirrorOnce = 5
  };

  class SamplerState
  {
    enum class PreDefined : uint8_t
    {
      WrapLinear,
      WrapAnisotropic,

      Num
    };
  public:
    using DataType = uint64_t;
  public:
#pragma warning(disable:4201)
    union
    {
      struct
      {
        AddressMode AddressModeU_ : 4;
        AddressMode AddressModeV_ : 4;
        AddressMode AddressModeW_ : 8;
        uint8_t MaxAnisotropy_ : 8;
        Filter Filter_ : 8;
        uint8_t BorderColorR_ : 8;
        uint8_t BorderColorG_ : 8;
        uint8_t BorderColorB_ : 8;
        uint8_t BorderColorA_ : 8;
        //Color BorderColor_;
        //  ↑operator=が暗黙的に削除される為使用不可(?)
      };
      DataType DataType_;
    };
#pragma warning(default:4201)
  public:
    constexpr SamplerState(AddressMode _AddressModeU = AddressMode::Clamp,
      AddressMode _AddressModeV = AddressMode::Clamp,
      AddressMode _AddressModeW = AddressMode::Clamp,
      Filter _Filter = Filter::MinMagMipLinear,
      uint8_t _MaxAnisotropy = 16,
      const Color& _BorderColor = Color(Palette::Black, 0))
      : Filter_(_Filter),
      AddressModeU_(_AddressModeU),
      AddressModeV_(_AddressModeV),
      AddressModeW_(_AddressModeW),
      MaxAnisotropy_(_MaxAnisotropy),
      BorderColorR_(_BorderColor.R),
      BorderColorG_(_BorderColor.G),
      BorderColorB_(_BorderColor.B),
      BorderColorA_(_BorderColor.A)
    {

    }
  public:
    SamplerState(PreDefined _PreDefined)
    {
      static constexpr SamplerState PreDefineds[static_cast<UINT>(PreDefined::Num)] =
      {
        { AddressMode::Wrap, AddressMode::Wrap, AddressMode::Wrap, Filter::MinMagMipLinear },
        { AddressMode::Wrap, AddressMode::Wrap, AddressMode::Wrap, Filter::Anisotropic }
      };

      *this = PreDefineds[static_cast<UINT>(_PreDefined)];
    }
  public:
    bool operator==(const SamplerState& _SamplerState)const
    {
      return DataType_ == _SamplerState.DataType_;
    }
    bool operator!=(const SamplerState& _SamplerState)const
    {
      return !((*this) == _SamplerState);
    }
  public:
    static constexpr PreDefined Default2D = PreDefined::WrapLinear;
    static constexpr PreDefined Default3D = PreDefined::WrapAnisotropic;
  };
}
