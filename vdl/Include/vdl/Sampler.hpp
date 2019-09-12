#pragma once
#include "Types.hpp"

namespace vdl
{
  enum class SamplerFilter : uint8_t
  {
    eMinMagMipPoint,
    eMinMagPointMipLinear,
    eMinPointMagLinearMipPoint,
    eMinPointMagMipLinear,
    eMinLinearMagMipPoint,
    eMinLinearMagPointMipLinear,
    eMinMagLinearMipPoint,
    eMinMagMipLinear,
    eAnisotropic,
    eComparisonMinMagMipPoint,
    eComparisonMinMagPointMipLinear,
    eComparisonMinPointMagLinearMipPoint,
    eComparisonMinPointMagMipLinear,
    eComparisonMinLinearMagMipPoint,
    eComparisonMinLinearMagPointMipLinear,
    eComparisonMinMagLinearMipPoint,
    eComparisonMinMagMipLinear,
    eComparisonAnisotropic,
  };

  enum class AddressMode : uint8_t
  {
    eWrap,
    eMirror,
    eClamp,
    eBorder,
    eMirrorOnce,
  };

  enum class BorderColor : uint8_t
  {
    eTransparent,
    eBlack,
    eWhite,
  };

  struct Sampler
  {
  private:
    enum class PreDefined : uint8_t
    {
      eWrapLinear,
      eWrapAnisotropic,

      eNum
    };
  public:
    static constexpr PreDefined kDefault2D = PreDefined::eWrapLinear;
    static constexpr PreDefined kDefault3D = PreDefined::eWrapAnisotropic;
  public:
    using DataType = uint32_t;
  public:
#pragma warning(disable:4201)
    union
    {
      struct
      {
        AddressMode AddressModeU : 4;
        AddressMode AddressModeV : 4;
        AddressMode AddressModeW : 4;
        uint8_t MaxAnisotropy : 8;
        SamplerFilter Filter : 6;
        BorderColor Color : 6;
      };
      DataType Data;
    };
#pragma warning(default:4201)
  public:
    constexpr Sampler(AddressMode _AddressModeU = AddressMode::eClamp, AddressMode _AddressModeV = AddressMode::eClamp, AddressMode _AddressModeW = AddressMode::eClamp,
      SamplerFilter _Filter = SamplerFilter::eMinMagMipLinear, uint8_t _MaxAnisotropy = 16, BorderColor _BorderColor = BorderColor::eBlack)
      : Filter(_Filter), AddressModeU(_AddressModeU), AddressModeV(_AddressModeV),
      AddressModeW(_AddressModeW), MaxAnisotropy(_MaxAnisotropy), Color(_BorderColor) {}

    Sampler(PreDefined _PreDefined)
    {
      static constexpr Sampler PreDefineds[static_cast<UINT>(PreDefined::eNum)] =
      {
        { AddressMode::eWrap, AddressMode::eWrap, AddressMode::eWrap, SamplerFilter::eMinMagMipLinear },
        { AddressMode::eWrap, AddressMode::eWrap, AddressMode::eWrap, SamplerFilter::eAnisotropic }
      };

      *this = PreDefineds[static_cast<uint>(_PreDefined)];
    }
  public:
    [[nodiscard]] constexpr bool operator==(const Sampler& _Sampler)const noexcept { return Data == _Sampler.Data; }

    [[nodiscard]] constexpr bool operator!=(const Sampler& _Sampler)const noexcept { return Data != _Sampler.Data; }
  };
}
