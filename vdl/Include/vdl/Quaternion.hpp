#pragma once
#include "Type3.hpp"
#include "Angle.hpp"

#include <DirectXMath.h>

namespace vdl
{
  struct Quaternion
  {
    float x, y, z, w;
  public:
    Quaternion() = default;

    Quaternion(const DirectX::XMVECTOR& _v)noexcept
      : x(_v.m128_f32[0]), y(_v.m128_f32[1]), z(_v.m128_f32[2]), w(_v.m128_f32[3]) {}

    constexpr Quaternion(float _x, float _y, float _z, float _w)noexcept
      : x(_x), y(_y), z(_z), w(_w) {}
  public:
    operator DirectX::XMVECTOR()const noexcept { return { x, y, z, w }; }
  public:
    [[nodiscard]] constexpr bool operator==(const Quaternion& _q)const noexcept { return x == _q.x && y == _q.y && z == _q.z && w == _q.w; }

    [[nodiscard]] constexpr bool operator!=(const Quaternion& _q)const noexcept { return x != _q.x || y != _q.y || z != _q.z || w != _q.w; }
    
    [[nodiscard]] Quaternion operator*(const Quaternion& _q)const { return DirectX::XMQuaternionMultiply(*this, _q); }

    Quaternion& operator=(const Quaternion& _q)noexcept
    {
      x = _q.x;
      y = _q.y;
      z = _q.z;
      w = _q.w;

      return *this;
    }

    Quaternion& operator*=(const Quaternion& _q)
    {
      *this = DirectX::XMQuaternionMultiply(*this, _q);

      return *this;
    }
  public:
    [[nodiscard]] float Dot(const Quaternion& _q)const { return DirectX::XMQuaternionDot(*this, _q).m128_f32[0]; }

    [[nodiscard]] Quaternion Slerp(const Quaternion& _q, float _t)const { return DirectX::XMQuaternionSlerp(*this, _q, _t); }

    [[nodiscard]] Quaternion Inverse()const { return DirectX::XMQuaternionInverse(*this); }

    [[nodiscard]] Quaternion Normalize()const { return DirectX::XMQuaternionNormalize(*this); }
  public:
    [[nodiscard]] static constexpr Quaternion Identity()noexcept { return { 0.0f, 0.0f, 0.0f, 1.0f }; }

    [[nodiscard]] static Quaternion RotationAxis(const float3& _Axis, const Radian& _Angle)
    {
      return DirectX::XMQuaternionRotationAxis(Quaternion(_Axis.x, _Axis.y, _Axis.z, 0.0f), _Angle);
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
