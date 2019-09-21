#pragma once
#include "Types.hpp"

namespace vdl
{
  enum class FilterType : uint8_t
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

  enum class AddressModeType : uint8_t
  {
    eWrap,
    eMirror,
    eClamp,
    eBorder,
    eMirrorOnce,
  };

  enum class BorderColorType : uint8_t
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
        FilterType Filter : 5;
        AddressModeType AddressModeU : 3;
        AddressModeType AddressModeV : 3;
        AddressModeType AddressModeW : 3;
        BorderColorType BorderColor : 2;
        uint8_t MaxAnisotropy;
      };
      DataType Data;
    };
#pragma warning(default:4201)
  public:
    constexpr Sampler(AddressModeType _AddressModeU = AddressModeType::eClamp, AddressModeType _AddressModeV = AddressModeType::eClamp, AddressModeType _AddressModeW = AddressModeType::eClamp,
      FilterType _Filter = FilterType::eMinMagMipLinear, uint8_t _MaxAnisotropy = 16, BorderColorType _BorderColor = BorderColorType::eBlack)
      : Filter(_Filter), AddressModeU(_AddressModeU), AddressModeV(_AddressModeV),
      AddressModeW(_AddressModeW), MaxAnisotropy(_MaxAnisotropy), BorderColor(_BorderColor) {}

    Sampler(PreDefined _PreDefined)
    {
      static constexpr Sampler PreDefineds[static_cast<UINT>(PreDefined::eNum)] =
      {
        { AddressModeType::eWrap, AddressModeType::eWrap, AddressModeType::eWrap, FilterType::eMinMagMipLinear },
        { AddressModeType::eWrap, AddressModeType::eWrap, AddressModeType::eWrap, FilterType::eAnisotropic }
      };

      *this = PreDefineds[static_cast<uint>(_PreDefined)];
    }
  public:
    [[nodiscard]] constexpr bool operator==(const Sampler& _Sampler)const noexcept { return Data == _Sampler.Data; }

    [[nodiscard]] constexpr bool operator!=(const Sampler& _Sampler)const noexcept { return Data != _Sampler.Data; }
  };
  static_assert(sizeof(Sampler) == sizeof(Sampler::DataType));
}
