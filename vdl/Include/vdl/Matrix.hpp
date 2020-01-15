#pragma once
#include "Fwd.hpp"
#include "Type2.hpp"
#include "Type3.hpp"
#include "Type4.hpp"
#include "Quaternion.hpp"
#include "Angle.hpp"
#include "Macro.hpp"

#include <string>
#include <fstream>

namespace vdl
{
  struct Matrix
  {
#pragma warning(disable:4201)
    union
    {
      float4 r[4];
      struct
      {
        float _11, _12, _13, _14;
        float _21, _22, _23, _24;
        float _31, _32, _33, _34;
        float _41, _42, _43, _44;
      };
      float m[4][4];
    };
#pragma warning(default:4201)
  public:
    Matrix() = default;

    constexpr Matrix(float _m00, float _m01, float _m02, float _m03,
      float _m10, float _m11, float _m12, float _m13,
      float _m20, float _m21, float _m22, float _m23,
      float _m30, float _m31, float _m32, float _m33)noexcept
      : _11(_m00), _12(_m01), _13(_m02), _14(_m03)
      , _21(_m10), _22(_m11), _23(_m12), _24(_m13)
      , _31(_m20), _32(_m21), _33(_m22), _34(_m23)
      , _41(_m30), _42(_m31), _43(_m32), _44(_m33) {}

    constexpr Matrix(const float4& _r0, const float4& _r1, const float4& _r2, const float4& _r3)noexcept
      : _11(_r0.x), _12(_r0.y), _13(_r0.z), _14(_r0.w)
      , _21(_r1.x), _22(_r1.y), _23(_r1.z), _24(_r1.w)
      , _31(_r2.x), _32(_r2.y), _33(_r2.z), _34(_r2.w)
      , _41(_r3.x), _42(_r3.y), _43(_r3.z), _44(_r3.w) {}
  public:
    [[nodiscard]] constexpr bool operator==(const Matrix& _m)const noexcept { return r[0] == _m.r[0] && r[1] == _m.r[1] && r[2] == _m.r[2] && r[3] == _m.r[3]; }

    [[nodiscard]] constexpr bool operator!=(const Matrix& _m)const noexcept { return r[0] != _m.r[0] || r[1] != _m.r[1] || r[2] != _m.r[2] || r[3] != _m.r[3]; }

    [[nodiscard]] constexpr Matrix operator*(const Matrix& _m)const noexcept
    {
      return {
        _11 * _m._11 + _12 * _m._21 + _13 * _m._31 + _14 * _m._41,
        _11 * _m._12 + _12 * _m._22 + _13 * _m._32 + _14 * _m._42,
        _11 * _m._13 + _12 * _m._23 + _13 * _m._33 + _14 * _m._43,
        _11 * _m._14 + _12 * _m._24 + _13 * _m._34 + _14 * _m._44,

        _21 * _m._11 + _22 * _m._21 + _23 * _m._31 + _24 * _m._41,
        _21 * _m._12 + _22 * _m._22 + _23 * _m._32 + _24 * _m._42,
        _21 * _m._13 + _22 * _m._23 + _23 * _m._33 + _24 * _m._43,
        _21 * _m._14 + _22 * _m._24 + _23 * _m._34 + _24 * _m._44,

        _31 * _m._11 + _32 * _m._21 + _33 * _m._31 + _34 * _m._41,
        _31 * _m._12 + _32 * _m._22 + _33 * _m._32 + _34 * _m._42,
        _31 * _m._13 + _32 * _m._23 + _33 * _m._33 + _34 * _m._43,
        _31 * _m._14 + _32 * _m._24 + _33 * _m._34 + _34 * _m._44,

        _41 * _m._11 + _42 * _m._21 + _43 * _m._31 + _44 * _m._41,
        _41 * _m._12 + _42 * _m._22 + _43 * _m._32 + _44 * _m._42,
        _41 * _m._13 + _42 * _m._23 + _43 * _m._33 + _44 * _m._43,
        _41 * _m._14 + _42 * _m._24 + _43 * _m._34 + _44 * _m._44
      };
    }

    Matrix& operator*=(const Matrix& _m)noexcept
    {
      return *this = *this * _m;
    }
  public:
    [[nodiscard]] constexpr float3 Right()const noexcept { return { _11, _12, _13 }; }

    [[nodiscard]] constexpr float3 Left()const noexcept { return { -_11, -_12, -_13 }; }

    [[nodiscard]] constexpr float3 Up()const noexcept { return { _21, _22, _23 }; }

    [[nodiscard]] constexpr float3 Down()const noexcept { return { -_21, -_22, -_23 }; }

    [[nodiscard]] constexpr float3 Forward()const noexcept { return { _31, _32, _33 }; }

    [[nodiscard]] constexpr float3 Back()const noexcept { return { -_31,-_32, -_33 }; }

    [[nodiscard]] Matrix Inverse()const noexcept
    {
      //  ë|Ç´èoÇµñ@
      //  https://thira.plavox.info/blog/2008/06/_c.html

      Matrix Original = *this;
      Matrix Inverse = Identity();
      {
        float Temp;
        uint i, j, k;
        for (i = 0; i < 4; ++i)
        {
          Temp = 1.0f / Original.r[i][i];
          for (j = 0; j < 4; ++j)
          {
            Original.r[i][j] *= Temp;
            Inverse.r[i][j] *= Temp;
          }

          for (j = 0; j < 4; ++j)
          {
            if (i == j)
            {
              continue;
            }

            Temp = Original.r[j][i];
            for (k = 0; k < 4; ++k)
            {
              Original.r[j][k] -= Original.r[i][k] * Temp;
              Inverse.r[j][k] -= Inverse.r[i][k] * Temp;
            }
          }
        }
      }

      return Inverse;
    }

    [[nodiscard]] constexpr Matrix Transpose()const noexcept
    {
      return {
        _11, _21, _31, _41,
        _12, _22, _32, _42,
        _13, _23, _33, _43,
        _14, _24, _34, _44
      };
    }
  public:
    [[nodiscard]] static constexpr Matrix Identity()noexcept
    {
      return {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };
    }

    [[nodiscard]] static constexpr Matrix Translate(const float3& _Pos)noexcept
    {
      return {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        _Pos.x, _Pos.y, _Pos.z, 1.0f
      };
    }

    [[nodiscard]] static constexpr Matrix Scale(const float3& _Scale)noexcept
    {
      return {
        _Scale.x, 0.0f, 0.0f, 0.0f,
        0.0f, _Scale.y, 0.0f, 0.0f,
        0.0f, 0.0f, _Scale.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      };
    }

    [[nodiscard]] static Matrix RotateX(const Radian& _Angle)noexcept
    {
      const float Cos = std::cos(_Angle);
      const float Sin = std::sin(_Angle);

      return {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, Cos, Sin, 0.0f,
        0.0f, -Sin, Cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };
    }

    [[nodiscard]] static Matrix RotateY(const Radian& _Angle)noexcept
    {
      const float Cos = std::cos(_Angle);
      const float Sin = std::sin(_Angle);

      return { Cos, 0.0f, -Sin, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        Sin, 0.0f, Cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };
    }

    [[nodiscard]] static Matrix RotateZ(const Radian& _Angle)noexcept
    {
      const float Cos = std::cos(_Angle);
      const float Sin = std::sin(_Angle);

      return { Cos, Sin, 0.0f, 0.0f,
        -Sin, Cos, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };
    }

    [[nodiscard]] static Matrix Rotate(const Radian& _Pitch, const Radian& _Yaw, const Radian& _Roll)noexcept
    {
      return RotateZ(_Roll) * RotateY(_Yaw) * RotateX(_Pitch);
    }

    [[nodiscard]] static constexpr Matrix Rotate(const Quaternion& _Orientation)noexcept
    {
      const float xx = _Orientation.x * _Orientation.x;
      const float xy = _Orientation.x * _Orientation.y;
      const float xz = _Orientation.x * _Orientation.z;
      const float yy = _Orientation.y * _Orientation.y;
      const float yz = _Orientation.y * _Orientation.z;
      const float zz = _Orientation.z * _Orientation.z;
      const float wx = _Orientation.w * _Orientation.x;
      const float wy = _Orientation.w * _Orientation.y;
      const float wz = _Orientation.w * _Orientation.z;

      return {
        1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f,
        2.0f * (xy - wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz * wx), 0.0f,
        2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (xx + yy), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      };
    }
  };

  [[nodiscard]] inline constexpr float4 operator*(const float4& _v, const Matrix& _m)
  {
    return {
      _v.x * _m._11 + _v.y * _m._21 + _v.z * _m._31 + _v.w * _m._41,
      _v.x * _m._12 + _v.y * _m._22 + _v.z * _m._32 + _v.w * _m._42,
      _v.x * _m._13 + _v.y * _m._23 + _v.z * _m._33 + _v.w * _m._43,
      _v.x * _m._14 + _v.y * _m._24 + _v.z * _m._34 + _v.w * _m._44 };
  }

  [[nodiscard]] inline constexpr float3 operator*(const float3& _v, const Matrix& _m)
  {
    const float4 v = float4(_v, 1.0f) * _m;
    return v.xyz() / v.w;
  }
}

namespace std
{
  [[nodiscard]] inline string to_string(const vdl::Matrix& _m)
  {
    return '('
      + to_string(_m.r[0]) + ','
      + to_string(_m.r[1]) + ','
      + to_string(_m.r[2]) + ','
      + to_string(_m.r[3]) + ')';
  }

  [[nodiscard]] inline wstring to_wstring(const vdl::Matrix& _m)
  {
    return L'('
      + to_wstring(_m.r[0]) + L','
      + to_wstring(_m.r[1]) + L','
      + to_wstring(_m.r[2]) + L','
      + to_wstring(_m.r[3]) + L')';
  }

  template <class CharType>
  inline basic_ostream<CharType>& operator<<(basic_ostream<CharType>& _OStream, const vdl::Matrix& _m)
  {
    return _OStream << CharType('(')
      << _m.r[0] << CharType(',')
      << _m.r[1] << CharType(',')
      << _m.r[2] << CharType(',')
      << _m.r[3] << CharType(')');
  }

  template <class CharType>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::Matrix& _m)
  {
    CharType Temp;

    return _IStream >> Temp
      >> _m.r[0] >> Temp
      >> _m.r[1] >> Temp
      >> _m.r[2] >> Temp
      >> _m.r[3] >> Temp;
  }
}
