#pragma once
#include "Fwd.hpp"
#include "Math.hpp"
#include "Type3.hpp"
#include "Angle.hpp"
#include "SIMD.hpp"

namespace vdl
{
  struct Quaternion
  {
#pragma warning(disable:4201)
    union
    {
      struct
      {
        float x, y, z, w;
      };
      SIMD_float4 s;
    };
#pragma warning(default:4201)
  public:
    Quaternion() = default;

    constexpr Quaternion(float _x, float _y, float _z, float _w)noexcept
      : x(_x), y(_y), z(_z), w(_w) {}

    Quaternion(const SIMD_float4& _s)noexcept
    {
      s = _s;
    }
  public:
    [[nodiscard]] constexpr bool operator==(const Quaternion& _q)const noexcept
    {
      return x == _q.x && y == _q.y && z == _q.z && w == _q.w;
    }

    [[nodiscard]] constexpr bool operator!=(const Quaternion& _q)const noexcept
    {
      return x != _q.x || y != _q.y || z != _q.z || w != _q.w;
    }

    [[nodiscard]] constexpr Quaternion operator*(const Quaternion& _q)const noexcept
    {
#if !defined _DEBUG && defined __cpp_lib_is_constant_evaluated
      if (!std::is_constant_evaluated())
      {
        return s.Swizzle<0, 0, 0, 0>() * SIMD_float4(1.0f, 1.0f, -1.0f, -1.0f) * _q.s.Swizzle<3, 2, 1, 0>()
          + s.Swizzle<1, 1, 1, 1>() * SIMD_float4(-1.0f, 1.0f, 1.0f, -1.0f) * _q.s.Swizzle<2, 3, 0, 1>()
          + s.Swizzle<2, 2, 2, 2>() * SIMD_float4(1.0f, -1.0f, 1.0f, -1.0f) * _q.s.Swizzle<1, 0, 3, 2>()
          + s.Swizzle<3, 3, 3, 3>() * _q.s;
      }
#endif
      return {
        x * _q.w - y * _q.z + z * _q.y + w * _q.x,
        x * _q.z + y * _q.w - z * _q.x + w * _q.y,
        -x * _q.y + y * _q.x + z * _q.w + w * _q.z,
        -x * _q.x - y * _q.y - z * _q.z + w * _q.w
      };
    }

    Quaternion& operator*=(const Quaternion& _q)noexcept
    {
      return *this = *this * _q;
    }
  public:
    [[nodiscard]] constexpr float Dot(const Quaternion& _q)const noexcept
    {
      return x * _q.x + y * _q.y + z * _q.z + w * _q.w;
    }

    [[nodiscard]] constexpr Quaternion Slerp(const Quaternion& _q, float _t)const noexcept
    {
      const float Angle = Math::ACos(Dot(_q));
      //  0èúéZÇ…Ç»ÇÈà◊ÅA0Ç≈ï‘Ç∑
      if (Angle == 0.0f)
      {
        return { 0.0f, 0.0f, 0.0f, 0.0f };
      }

      const float Sin = Math::Sin(Angle);
#if !defined _DEBUG && defined __cpp_lib_is_constant_evaluated
      if (!std::is_constant_evaluated())
      {
        const SIMD_float4 Sin0 = Math::Sin(Angle * (1.0f - _t)) / Sin;
        const SIMD_float4 Sin1 = Math::Sin(Angle * _t) / Sin;

        return s * Sin0 + _q.s * Sin1;
      }
#endif
      const float Sin0 = Math::Sin(Angle * (1.0f - _t)) / Sin;
      const float Sin1 = Math::Sin(Angle * _t) / Sin;

      return { x * Sin0 + _q.x * Sin1, y * Sin0 + _q.y * Sin1, z * Sin0 + _q.z * Sin1, w * Sin0 + _q.w * Sin1 };
    }

    [[nodiscard]] constexpr Quaternion Inverse()const
    {
      const float LengthSq = x * x + y * y + z * z + w * w;
      return { -x / LengthSq, -y / LengthSq, -z / LengthSq, w / LengthSq };
    }

    [[nodiscard]] constexpr Quaternion Normalize()const noexcept
    {
#if !defined _DEBUG && defined __cpp_lib_is_constant_evaluated
      if (!std::is_constant_evaluated())
      {
        return s / s.Length();
      }
#endif
      const float Length = Math::Sqrt(x * x + y * y + z * z + w * w);
      return { x / Length, y / Length , z / Length , w / Length };
    }
  public:
    [[nodiscard]] static constexpr Quaternion Identity()noexcept
    {
      return { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    [[nodiscard]] static constexpr Quaternion RotationAxis(const float3& _Axis, const Radian& _Angle)noexcept
    {
      const float Cos = Math::Cos(_Angle * 0.5f);
      const float Sin = Math::Sin(_Angle * 0.5f);

      return { _Axis.x * Sin, _Axis.y * Sin, _Axis.z * Sin, Cos };
    }
  };
}

namespace std
{
  [[nodiscard]] inline string to_string(const vdl::Quaternion& _q)
  {
    return to_string(_q.x) + ',' + to_string(_q.y) + ',' + to_string(_q.z) + ',' + to_string(_q.w);
  }

  [[nodiscard]] inline wstring to_wstring(const vdl::Quaternion& _q)
  {
    return to_wstring(_q.x) + L',' + to_wstring(_q.y) + L',' + to_wstring(_q.z) + L',' + to_wstring(_q.w);
  }

  template <class CharType>
  inline basic_ostream<CharType>& operator<<(basic_ostream<CharType>& _OStream, const vdl::Quaternion& _q)
  {
    return _OStream << _q.x << static_cast<CharType>(',') << _q.y << static_cast<CharType>(',') << _q.z << static_cast<CharType>(',') << _q.w;
  }

  template <class CharType>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::Quaternion& _q)
  {
    CharType Temp;
    return _IStream >> _q.x >> Temp >> _q.y >> Temp >> _q.z >> Temp >> _q.w;
  }
}
