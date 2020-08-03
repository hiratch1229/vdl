#pragma once
#include "MathConstants.hpp"

#include <cmath>

namespace vdl::Math
{
  //  数値がNan(Not a Number)ならtrueを返します
  template<class T, std::enable_if_t<std::is_fundamental<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr bool isNan(T _x)noexcept
  {
    //  Nan同士の比較はfalseを返すのを利用
    return !(_x == _x);
  }

  //  数値が無限大(Infinity)ならtrueを返します
  template<class T, std::enable_if_t<std::is_fundamental<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr bool isInf(T _x)noexcept
  {
    return _x == std::numeric_limits<T>::infinity() || _x == -std::numeric_limits<T>::infinity();
  }

  //  数値が有限(Finite Value)ならtrueを返します
  template<class T, std::enable_if_t<std::is_fundamental<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr bool isFinite(T _x)noexcept
  {
    return !(isNan(_x) || isInf(_x));
  }

  //----------------------------------------------------------------------------------------------------

  //  負の値ならtrueを返します
  template<class T, std::enable_if_t<std::is_fundamental<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr bool SignBit(T _x)noexcept
  {
    return _x < 0;
  }

  //  絶対値を返します
  template<class T, std::enable_if_t<std::is_fundamental<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T Abs(T _x)noexcept
  {
    return (_x < 0 ? -_x : _x);
  }

  //  xの絶対値にyの符号が付いた値を返します
  template<class T, std::enable_if_t<std::is_fundamental<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T CopySign(T _x, T _y)noexcept
  {
    T AbsValue = isNan(_x) ? std::numeric_limits<T>::quiet_NaN() : Abs(_x);
    return _y >= 0 ? AbsValue : -AbsValue;
  }

  //----------------------------------------------------------------------------------------------------

  //  階乗を返します
  [[nodiscard]] inline constexpr uint Factorial(uint _x)noexcept
  {
    if (_x <= 1)
    {
      return 1;
    }

    uint Sum = 2;
    for (uint i = 3; i <= _x; ++i)
    {
      Sum *= i;
    }
    return Sum;
  }

  //  累乗を返します
  template<class T, std::enable_if_t<std::is_fundamental<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T Pow(T _x, int _y)noexcept
  {
#if defined __cpp_lib_is_constant_evaluated
    if (!std::is_constant_evaluated())
    {
      return static_cast<T>(std::pow(_x, _y));
    }
#endif
    if constexpr (std::numeric_limits<T>::is_iec559)
    {
      if (_x == 0)
      {
        if (_y < 0 && _y % 2 == 1)
        {
          return CopySign(std::numeric_limits<T>::infinity(), _x);
        }
        else if (isFinite(_y))
        {
          return std::numeric_limits<T>::infinity();
        }
        if (_y == -std::numeric_limits<T>::infinity())
        {
          return std::numeric_limits<T>::infinity();
        }
        if (_y > 0)
        {
          if (_y % 2 == 1)
          {
            return _x;
          }
          else
          {
            return static_cast<T>(0);
          }
        }
      }
      else if (_x == -1 && isInf(_y))
      {
        return static_cast<T>(1);
      }
      else if (_x == 1)
      {
        return static_cast<T>(1);
      }
      else if (_y == 0)
      {
        return static_cast<T>(1);
      }
      else if (Abs(_x) < 1)
      {
        if (_y == -std::numeric_limits<T>::infinity())
        {
          return std::numeric_limits<T>::infinity();
        }
        else if (_y == std::numeric_limits<T>::infinity())
        {
          return static_cast<T>(0);
        }
      }
      else if (Abs(_x) > 1)
      {
        if (_y == -std::numeric_limits<T>::infinity())
        {
          return static_cast<T>(0);
        }
        else if (_y == std::numeric_limits<T>::infinity())
        {
          return std::numeric_limits<T>::infinity();
        }
      }
      else if (_x == -std::numeric_limits<T>::infinity())
      {
        if (_y < 0)
        {
          if (_y % 2 == 1)
          {
            return static_cast<T>(-0);
          }
          else
          {
            return static_cast<T>(0);
          }
        }
        else if (_y > 0)
        {
          if (_y % 2 == 1)
          {
            return -std::numeric_limits<T>::infinity();
          }
          else
          {
            return std::numeric_limits<T>::infinity();
          }
        }
      }
      else if (_x == std::numeric_limits<T>::infinity())
      {
        if (_y < 0)
        {
          return static_cast<T>(0);
        }
        else if (_y > 0)
        {
          return std::numeric_limits<T>::infinity();
        }
      }
    }

    double Sum = 1;
    if (_y < 0)
    {
      _y = -_y;
      _x = 1 / _x;
    }

    for (int i = 1; i <= _y; ++i)
    {
      Sum *= _x;
    }

    return static_cast<T>(Sum);
  }

  //  非負の平方根を返します
  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T Sqrt(T _x)noexcept
  {
#if defined __cpp_lib_is_constant_evaluated
    if (!std::is_constant_evaluated())
    {
      return std::sqrt(_x);
    }
#endif
    if (_x < 0)
    {
      return -std::numeric_limits<T>::quiet_NaN();
    }
    if (_x == 0)
    {
      return _x;
    }

    double y = 1.0;
    double Result = 0.0;
    for (uint i = 0; ; ++i)
    {
      Result = (y + _x / y) / 2.0;

      //  精度問題で数値が変動しなくなったら終了
      if (isInf(Result) || y == Result)
      {
        return static_cast<T>(Result);
      }

      y = Result;
    }
  }

  //  e(ネイピア数)の累乗を返します
  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T Exp(T _x)noexcept
  {
#if defined __cpp_lib_is_constant_evaluated
    if (!std::is_constant_evaluated())
    {
      return std::exp(_x);
    }
#endif
    if constexpr (std::numeric_limits<T>::is_iec559)
    {
      if (_x == 0)
      {
        return static_cast<T>(1);
      }
      if (_x == -std::numeric_limits<T>::infinity())
      {
        return static_cast<T>(0);
      }
      if (_x == std::numeric_limits<T>::infinity())
      {
        return std::numeric_limits<T>::infinity();
      }
    }

    double Sum = 1.0;
    double k = 0.0;

    for (uint i = 1;; ++i)
    {
      k = Pow(_x, i) / Factorial(i);

      //  精度問題で数値が変動しなくなったら終了
      if (isInf(Sum + k) || Sum == Sum + k)
      {
        return static_cast<T>(Sum);
      }

      Sum += k;
    }
  }

  //  //  e(ネイピア数)を底とする自然対数を返します
  //  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  //  [[nodiscard]] inline constexpr T Log(T _x)noexcept
  //  {
  //#if defined __cpp_lib_is_constant_evaluated
  //    if (!std::is_constant_evaluated())
  //    {
  //      return std::log(_x);
  //    }
  //#endif
  //    if constexpr (std::numeric_limits<T>::is_iec559)
  //    {
  //      if (_x == 0)
  //      {
  //        return -std::numeric_limits<T>::infinity();
  //      }
  //      if (_x == 1)
  //      {
  //        return static_cast<T>(0);
  //      }
  //      if (_x < 0)
  //      {
  //        return -std::numeric_limits<T>::quiet_NaN();
  //      }
  //      if (_x == std::numeric_limits<T>::infinity())
  //      {
  //        return std::numeric_limits<T>::infinity();
  //      }
  //    }
  //
  //    //  Logの実計算関数
  //    constexpr auto Calc = [](T _x)->T
  //    {
  //      double Sum = 0.0;
  //      double k = 0.0;
  //
  //      for (uint i = 1;; ++i)
  //      {
  //        k = ((i % 2 == 0 ? -1.0 : 1.0) * Pow(_x, i)) / i;
  //
  //        //  無限大になるか精度問題で数値が変動しなくなったら終了
  //        if (isInf(Sum + k) || Sum == Sum + k)
  //        {
  //          return static_cast<T>(Sum);
  //        }
  //
  //        Sum += k;
  //      }
  //    };
  //
  //    constexpr T kNapier = static_cast<T>(2.71828182846);
  //    return Calc(_x) / kNapier;
  //}
  //
  ////  2を底とする二進対数を返します
  //template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  //[[nodiscard]] inline constexpr T Log2(T _x)noexcept
  //{
  //  constexpr T kDenominator = Log(static_cast<T>(2));
  //  return Log(_x) / kDenominator;
  //}
  //
  ////  10を底とする常用対数を返します
  //template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  //[[nodiscard]] inline constexpr T Log10(T _x)noexcept
  //{
  //  constexpr T kDenominator = Log(static_cast<T>(10));
  //  return Log(_x) / kDenominator;
  //}

  //----------------------------------------------------------------------------------------------------

  //  正弦を返します
  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T Sin(T _x)noexcept
  {
#if defined __cpp_lib_is_constant_evaluated
    if (!std::is_constant_evaluated())
    {
      return std::sin(_x);
    }
#endif
    if constexpr (std::numeric_limits<T>::is_iec559)
    {
      if (_x == 0)
      {
        return static_cast<T>(0);
      }
      if (isInf(_x))
      {
        return std::numeric_limits<T>::quiet_NaN();
      }
    }

    // -2π～2πにする
    _x -= static_cast<int>(_x / (2 * kPI<T>)) * 2 * kPI<T>;

    double Sum = _x;
    double k = _x;
    for (uint i = 2;; i += 2)
    {
      k *= -(_x * _x) / (i * (i + 1));

      //  無限大になるか精度問題で数値が変動しなくなったら終了
      if (isInf(Sum + k) || Sum == Sum + k)
      {
        return static_cast<T>(Sum);
      }

      Sum += k;
    }
  }

  //  余弦を返します
  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T Cos(T _x)noexcept
  {
#if defined __cpp_lib_is_constant_evaluated
    if (!std::is_constant_evaluated())
    {
      return std::cos(_x);
    }
#endif
    if constexpr (std::numeric_limits<T>::is_iec559)
    {
      if (_x == 0)
      {
        return static_cast<T>(1);
      }
      if (isInf(_x))
      {
        return std::numeric_limits<T>::quiet_NaN();
      }
    }

    return static_cast<T>(Sin(kHalfPI<double> -_x));
  }

  //  正接を返します
  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T Tan(T _x)noexcept
  {
#if defined __cpp_lib_is_constant_evaluated
    if (!std::is_constant_evaluated())
    {
      return std::tan(_x);
    }
#endif
    if constexpr (std::numeric_limits<T>::is_iec559)
    {
      if (_x == 0)
      {
        return 0;
      }
      if (isInf(_x))
      {
        return std::numeric_limits<T>::quiet_NaN();
      }
    }

    return static_cast<T>(Sin(static_cast<double>(_x)) / Cos(static_cast<double>(_x)));
  }

  //  逆正弦を返します
  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T ASin(T _x)noexcept
  {
#if defined __cpp_lib_is_constant_evaluated
    if (!std::is_constant_evaluated())
    {
      return std::asin(_x);
    }
#endif
    if constexpr (std::numeric_limits<T>::is_iec559)
    {
      if (_x == 0)
      {
        return static_cast<T>(0);
      }
      if (Abs(_x) > 1)
      {
        return std::numeric_limits<T>::quiet_NaN();
      }
    }

    // -2π～2πにする
    _x -= static_cast<int>(_x / (2 * kPI<T>)) * 2 * kPI<T>;

    //  ASinの実計算関数
    constexpr auto Calc = [](T _x)->T
    {
      double Sum = _x;
      double k = 0.0;
      for (uint i = 1;; ++i)
      {
        k = (Factorial(2 * i) * Pow(_x, 2 * i + 1)) / (Pow(4.0, i) * Pow(Factorial(i), 2) * (2 * i + 1));

        //  無限大になるか精度問題で数値が変動しなくなったら終了
        if (isInf(Sum + k) || Sum == Sum + k)
        {
          return static_cast<T>(Sum);
        }

        Sum += k;
      }
    };

    const T Sign = static_cast<T>(_x < 0 ? -1 : 1);
    _x = Abs(_x);

    //  精度問題による公式の変換
    return Sign * (1 / Sqrt(static_cast<T>(2)) < _x && _x <= static_cast<T>(1) ? kHalfPI<T> -Calc(Sqrt(1 - _x * _x)) : Calc(_x));
  }

  //  逆余弦を返します
  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T ACos(T _x)noexcept
  {
#if defined __cpp_lib_is_constant_evaluated
    if (!std::is_constant_evaluated())
    {
      return std::acos(_x);
    }
#endif
    if constexpr (std::numeric_limits<T>::is_iec559)
    {
      if (_x == 1)
      {
        return static_cast<T>(0);
      }
      if (Abs(_x) > 1)
      {
        return std::numeric_limits<T>::quiet_NaN();
      }
    }

    return kHalfPI<T> -ASin(_x);
  }

  //  逆正接を返します
  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T ATan(T _x)noexcept
  {
#if defined __cpp_lib_is_constant_evaluated
    if (!std::is_constant_evaluated())
    {
      return std::atan(_x);
    }
#endif
    if constexpr (std::numeric_limits<T>::is_iec559)
    {
      if (_x == 0)
      {
        return static_cast<T>(0);
      }
      if (isInf(_x))
      {
        return CopySign(kHalfPI<T>, _x);
      }
    }

    // -2π～2πにする
    _x -= static_cast<int>(_x / (2 * kPI<T>)) * 2 * kPI<T>;

    //  ATanの実計算関数
    constexpr auto Calc = [](T _x)->T
    {
      double Sum = _x;
      double k = _x;

      for (uint i = 1;; ++i)
      {
        k = ((i % 2 == 0 ? 1 : -1) * Pow(_x, 2 * i + 1)) / (2 * i + 1);

        //  無限大になるか精度問題で数値が変動しなくなったら終了
        if (isInf(Sum + k) || Sum == Sum + k)
        {
          return static_cast<T>(Sum);
        }

        Sum += k;
      }
    };

    const T Sign = static_cast<T>(_x < 0 ? -1 : 1);
    _x = Abs(_x);

    //  精度問題による公式の変換
    return Sign * (_x > Sqrt(static_cast<T>(2)) + 1 ? kHalfPI<T> -Calc(1 / _x)
      : _x > Sqrt(static_cast<T>(2)) - 1 ? kPI<T> / 4 + Calc((_x - 1) / (_x + 1)) : Calc(_x));
  }

  //  逆正接を返します
  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T ATan2(T _y, T _x)noexcept
  {
#if defined __cpp_lib_is_constant_evaluated
    if (!std::is_constant_evaluated())
    {
      return std::atan2(_y, _x);
    }
#endif
    if constexpr (std::numeric_limits<T>::is_iec559)
    {
      if (isInf(_y))
      {
        if (_x == std::numeric_limits<T>::infinity())
        {
          return CopySign(3 * kPI<T> / 4, _y);
        }
        else if (_x == -std::numeric_limits<T>::infinity())
        {
          return CopySign(kPI<T> / 4, _y);
        }
        else
        {
          return CopySign(kHalfPI<T>, _y);
        }
      }

      if (_x < 0)
      {
        if (0 <= _y)
        {
          return ATan(_y / _x) + kPI<T>;
        }
        else
        {
          return ATan(_y / _x) - kPI<T>;
        }
      }
      else if (_x == 0)
      {
        if (_y < 0)
        {
          return -kHalfPI<T>;
        }
        else if (0 < _y)
        {
          return kHalfPI<T>;
        }
        else
        {
          return static_cast<T>(0);
        }
      }
      else
      {
        return ATan(_y / _x);
      }
    }
    else
    {
      if (_x < 0)
      {
        if (0 <= _y)
        {
          return ATan(_y / _x) + kPI<T>;
        }
        else
        {
          return ATan(_y / _x) - kPI<T>;
        }
      }
      else
      {
        return ATan(_y / _x);
      }
    }
  }

  //----------------------------------------------------------------------------------------------------

  //  ラジアン(Radian)を度数法(Degree)に変換
  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T ToDegree(T _Radian)noexcept
  {
    return _Radian * (static_cast<T>(180) / kPI<T>);
  }

  //  度数法(Degree)をラジアン(Radian)に変換
  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T ToRadian(T _Degree)noexcept
  {
    return _Degree * (kPI<T> / static_cast<T>(180));
  }

  //----------------------------------------------------------------------------------------------------

  //  epsilonの誤差を考慮した比較
  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr bool isEqual(T _x, T _y, T _epsilon = std::numeric_limits<T>::epsilon())
  {
    return ((_y - _epsilon) < _x) && (_x < (_y + _epsilon));
  }

  //  値を範囲内に収めます
  template<class T>
  [[nodiscard]] inline constexpr const T& Clamp(const T& _x, const T& _min, const T& _max)
  {
    return (_x < _min ? _min : _max < _x ? _max : _x);
  }

  //  値を0~1に収めます
  template<class T>
  [[nodiscard]] inline constexpr T Saturate(T _x)
  {
    return (_x < static_cast<T>(0) ? static_cast<T>(0) : static_cast<T>(1) < _x ? static_cast<T>(1) : _x);
  }

  //  aとbの間をtで線形補間した値を返します
  template<class T, std::enable_if_t<std::is_floating_point<T>::value, std::nullptr_t> = nullptr>
  [[nodiscard]] inline constexpr T Lerp(T a, T b, T t)noexcept
  {
    return a + t * (b - a);
  }

  //  最小値を返します
  template<class T, class U>
  [[nodiscard]] inline constexpr auto Min(T x, U y)noexcept->decltype(x + y)
  {
    return (x < y ? x : y);
  }

  template<class T, class... Arg>
  [[nodiscard]] inline constexpr auto Min(T x, Arg ...args)noexcept->decltype(x + Min(args...))
  {
    const auto MinValue = Min(args...);
    return (x < MinValue ? x : MinValue);
  }

  //  最大値を返します
  template<class T, class U>
  [[nodiscard]] inline constexpr auto Max(T x, U y)noexcept->decltype(x + y)
  {
    return (x < y ? y : x);
  }

  template<class T, class... Arg>
  [[nodiscard]] inline constexpr auto Max(T x, Arg ...args)noexcept->decltype(x + Max(args...))
  {
    const auto MaxValue = Max(args...);
    return (x < MaxValue ? MaxValue : x);
  }
}
