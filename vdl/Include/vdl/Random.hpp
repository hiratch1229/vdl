#pragma once
#include "Type4.hpp"
#include <assert.h>

namespace vdl
{
  class Random
  {
    static constexpr uint4 kInitSeedValue = { 123456789u, 362436069u, 521288629u, 88675123u };
  private:
    inline static uint4 Data = kInitSeedValue;
  private:
    Random() = default;
  private:
    static uint Rand()
    {
      uint Temp = (Data.x ^ (Data.x << 11));
      Data.x = Data.y;
      Data.y = Data.z;
      Data.z = Data.w;

      return Data.w = (Data.w ^ (Data.w >> 19)) ^ (Temp ^ (Temp >> 8));
    }
  public:
    [[nodiscard]] static int Range(int _Min, int _Max)
    {
      const uint Value = Rand();

      if (_Min > _Max)
      {
        const int Temp = _Min;
        _Min = _Max;
        _Max = Temp;
      }

      return Value % (_Max - _Min) + _Min;
    }

    [[nodiscard]] static float Range(float _Min, float _Max)
    {
      const uint Value = Rand();

      if (_Min > _Max)
      {
        const float Temp = _Min;
        _Min = _Max;
        _Max = Temp;
      }

      return (static_cast<float>(Value) / ~0u) * (_Max - _Min) + _Min;
    }

    [[nodiscard]] static void SetSeed(const uint4& _Seed = kInitSeedValue)
    {
      assert(_Seed == 0);

      Data = _Seed;
    }
  };
}
