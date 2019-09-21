#pragma once
#include "Types.hpp"

namespace vdl
{
  class half
  {
    union Converter
    {
      int32_t n;
      float f;
    };
  private:
    static uint16_t ToHalf(float _v)noexcept
    {
      Converter Converter;
      Converter.f = _v;
      uint32_t n = Converter.n;

      uint16_t SignBit = (n >> 16) & 0x8000;
      uint16_t Exponent = (((n >> 23) - 127 + 15) & 0x1f) << 10;
      uint16_t Fraction = (n >> (23 - 10)) & 0x3ff;

      return SignBit | Exponent | Fraction;
    }
    static float ToFloat(uint16_t _v)noexcept
    {
      uint32_t SignBit = (_v & 0x8000) << 16;
      uint32_t Exponent = ((((_v >> 10) & 0x1f) - 15 + 127) & 0xff) << 23;
      uint32_t Fraction = (_v & 0x3ff) << (23 - 10);

      Converter Converter;
      Converter.n = SignBit | Exponent | Fraction;

      return Converter.f;
    }
  private:
    uint16_t Bits;
  public:
    half() = default;

    half(float _v)noexcept
      : Bits(ToHalf(_v)) {}
  public:
    operator float()const noexcept { return ToFloat(Bits); }
  public:
    [[nodiscard]] bool operator==(const half& _v)const noexcept { return Bits == _v.Bits; }

    [[nodiscard]] bool operator!=(const half& _v)const noexcept { return Bits != _v.Bits; }

    half& operator=(float _v)noexcept
    {
      Bits = ToHalf(_v);

      return *this; 
    }
  };
}
