#pragma once
#include "Fwd.hpp"

namespace vdl
{
  template<class FlagType, class DataType = uint>
  class Flags
  {
    DataType Mask_;
  public:
    constexpr Flags()
      : Mask_(0) {}

    Flags(const Flags&) = default;

    Flags(Flags&&) = default;

    constexpr Flags(FlagType _Flag)noexcept
      : Mask_(static_cast<DataType>(_Flag)) {}

    constexpr Flags(DataType _Flags)noexcept
      : Mask_(_Flags) {}
  public:
    operator DataType()const { return Mask_; }
  public:
    Flags& operator=(const Flags&) = default;

    Flags& operator=(Flags&&) = default;

    Flags& operator|=(const Flags& _Flags)
    {
      Mask_ |= _Flags.Mask_;

      return *this;
    }

    Flags& operator&=(const Flags& _Flags)
    {
      Mask_ &= _Flags.Mask_;

      return *this;
    }

    Flags& operator^=(const Flags& _Flags)
    {
      Mask_ ^= _Flags.Mask_;

      return *this;
    }

    [[nodiscard]] constexpr bool operator==(const Flags& _Flags)const noexcept { return Mask_ == _Flags.Mask_; }

    [[nodiscard]] constexpr bool operator!=(const Flags& _Flags)const noexcept { return Mask_ != _Flags.Mask_; }

    [[nodiscard]] constexpr Flags operator|(const Flags& _Flags)const noexcept { return { static_cast<DataType>(Mask_ | _Flags.Mask_) }; }

    [[nodiscard]] constexpr Flags operator&(const Flags& _Flags)const noexcept { return { static_cast<DataType>(Mask_ & _Flags.Mask_) }; }

    [[nodiscard]] constexpr Flags operator^(const Flags& _Flags)const noexcept { return { static_cast<DataType>(Mask_ ^ _Flags.Mask_) }; }

    [[nodiscard]] constexpr Flags operator~()const noexcept { return { static_cast<DataType>(Mask_ ^ static_cast<DataType>(~0u)) }; }
  };
}
