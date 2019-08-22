#pragma once
#include "Type2.hpp"
#include <float.h>

namespace vdl
{
  class Radian;
}

namespace vdl::Math
{
  //  _Num0と_Num1を_Epsilonの誤差を考慮した比較
  inline constexpr bool isEqual(float _V0, float _V1, float _Epsilon = FLT_EPSILON)
  {
    return ((_V1 - _Epsilon) < _V0) && (_V0 < (_V1 + _Epsilon));
  }

  //  絶対値を返します
  template <class T>
  inline constexpr T GetAbsoluteValue(T _Num)
  {
    return (_Num < static_cast<T>(0)) ? -_Num : _Num;
  }

  //  値を範囲内に収めます
  template <class T>
  inline constexpr const T& Clamp(const T& _V, const T& _Low, const T& _High)
  {
    return (_Low <= _V && _V <= _High) ? _V : (_V < _Low) ? _Low : _High;
  }

  //  最小値を取得
  template <class T>
  inline constexpr const T& Min(const T& _Left, const T& _Right)
  {
    return (_Left < _Right) ? _Left : _Right;
  }

  //  最大値を取得
  template <class T>
  inline constexpr const T& Max(const T& _Left, const T& _Right)
  {
    return (_Left > _Right) ? _Left : _Right;
  }
}
