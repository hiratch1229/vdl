#pragma once
#include "Fwd.hpp"
#include "Type3.hpp"
#include "Angle.hpp"

namespace vdl
{
  struct Quaternion
  {
    float x, y, z, w;
  public:
    Quaternion() = default;

    constexpr Quaternion(float _x, float _y, float _z, float _w)noexcept
      : x(_x), y(_y), z(_z), w(_w) {}
  public:
    [[nodiscard]] constexpr bool operator==(const Quaternion& _q)const noexcept { return x == _q.x && y == _q.y && z == _q.z && w == _q.w; }

    [[nodiscard]] constexpr bool operator!=(const Quaternion& _q)const noexcept { return x != _q.x || y != _q.y || z != _q.z || w != _q.w; }

    [[nodiscard]] constexpr Quaternion operator*(const Quaternion& _q)const noexcept
    {
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
    [[nodiscard]] constexpr float Dot(const Quaternion& _q)const noexcept { return x * _q.x + y * _q.y + z * _q.z + w * _q.w; }

    [[nodiscard]] Quaternion Slerp(const Quaternion& _q, float _t)const noexcept
    {
      const float Angle = std::acos(Dot(_q));
      const float Sin = std::sin(Angle);

      const float Sin0 = std::sin(Angle * (1.0f - _t)) / Sin;
      const float Sin1 = std::sin(Angle * _t) / Sin;

      return { x * Sin0 + _q.x * Sin1, y * Sin0 + _q.y * Sin1, z * Sin0 + _q.z * Sin1, w * Sin0 + _q.w * Sin1, };
    }

    [[nodiscard]] Quaternion Inverse()const
    {
      const float LengthSq = x * x + y * y + z * z + w * w;
      return { -x / LengthSq, -y / LengthSq, -z / LengthSq, w / LengthSq };
    }

    [[nodiscard]] Quaternion Normalize()const noexcept
    {
      const float Length = std::sqrt(x * x + y * y + z * z + w * w);
      return { x / Length, y / Length , z / Length , w / Length };
    }
  public:
    [[nodiscard]] static constexpr Quaternion Identity()noexcept { return { 0.0f, 0.0f, 0.0f, 1.0f }; }

    [[nodiscard]] static Quaternion RotationAxis(const float3& _Axis, const Radian& _Angle)
    {
      const float Cos = std::cos(_Angle * 0.5f);
      const float Sin = std::sin(_Angle * 0.5f);

      return { _Axis.x * Sin,_Axis.y * Sin,_Axis.z * Sin, Cos };
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
    return _OStream << _q.x << CharType(',') << _q.y << CharType(',') << _q.z << CharType(',') << _q.w;
  }

  template <class CharType>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::Quaternion& _q)
  {
    CharType Temp;

    return _IStream >> _q.x >> Temp >> _q.y >> Temp >> _q.z >> Temp >> _q.w;
  }
}
