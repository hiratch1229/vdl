#pragma once
#include <float.h>

namespace vdl
{
  namespace Math
  {
    //  _Num0��_Num1��_Epsilon�̌덷���l��������r
    inline constexpr bool isEqual(float _V0, float _V1, float _Epsilon = FLT_EPSILON)
    {
      return ((_V1 - _Epsilon) < _V0) && (_V0 < (_V1 + _Epsilon));
    }

    //  ��Βl��Ԃ��܂�
    template <class T>
    inline constexpr T GetAbsoluteValue(T _Num)
    {
      return (_Num < static_cast<T>(0)) ? -_Num : _Num;
    }

    //  �l��͈͓��Ɏ��߂܂�
    template <class T>
    inline constexpr const T& Clamp(const T& _V, const T& _Low, const T& _High)
    {
      return (_Low <= _V && _V <= _High) ? _V : (_V < _Low) ? _Low : _High;
    }

    //  �ŏ��l���擾
    template <class T>
    inline constexpr const T& Min(const T& _Left, const T& _Right)
    {
      return (_Left < _Right) ? _Left : _Right;
    }

    //  �ő�l���擾
    template <class T>
    inline constexpr const T& Max(const T& _Left, const T& _Right)
    {
      return (_Left > _Right) ? _Left : _Right;
    }
  }
}
