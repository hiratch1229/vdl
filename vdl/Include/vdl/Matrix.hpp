#pragma once
#include "Fwd.hpp"
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
      SIMD_float4 s[4];
    };
#pragma warning(default:4201)
  public:
    Matrix() = default;

    Matrix(const Matrix&) = default;

    Matrix(Matrix&&) = default;

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

    Matrix(const SIMD_float4& _s0, const SIMD_float4& _s1, const SIMD_float4& _s2, const SIMD_float4& _s3)noexcept
    {
      s[0] = _s0;
      s[1] = _s1;
      s[2] = _s2;
      s[3] = _s3;
    }
  public:
    Matrix& operator=(const Matrix&) = default;

    Matrix& operator=(Matrix&&) = default;

    [[nodiscard]] constexpr float4& operator[](uint _Index) { return r[_Index]; }

    [[nodiscard]] constexpr const float4& operator[](uint _Index)const { return r[_Index]; }

    [[nodiscard]] constexpr bool operator==(const Matrix& _m)const noexcept { return r[0] == _m.r[0] && r[1] == _m.r[1] && r[2] == _m.r[2] && r[3] == _m.r[3]; }

    [[nodiscard]] constexpr bool operator!=(const Matrix& _m)const noexcept { return r[0] != _m.r[0] || r[1] != _m.r[1] || r[2] != _m.r[2] || r[3] != _m.r[3]; }

    [[nodiscard]] constexpr Matrix operator*(const Matrix& _m)const noexcept
    {
#if !defined _DEBUG && defined __cpp_lib_is_constant_evaluated
      if (!std::is_constant_evaluated())
      {
        Matrix m;

        SIMD_float4 x, y, z, w;
        for (uint i = 0; i < 4; ++i)
        {
          x = s[i].Swizzle<0, 0, 0, 0>();
          y = s[i].Swizzle<1, 1, 1, 1>();
          z = s[i].Swizzle<2, 2, 2, 2>();
          w = s[i].Swizzle<3, 3, 3, 3>();

          x *= _m.s[0];
          y *= _m.s[1];
          z *= _m.s[2];
          w *= _m.s[3];

          m.s[i] = x + y + z + w;
        }

        return m;
      }
#endif
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

    [[nodiscard]] constexpr Matrix Inverse()const noexcept
    {
#if !defined _DEBUG && __cpp_lib_is_constant_evaluated
      if (!std::is_constant_evaluated())
      {
        // sub matrices
        SIMD_float4 A = s[0].Shuffle<0, 1, 0, 1>(s[1]);
        SIMD_float4 B = s[0].Shuffle<3, 2, 2, 3>(s[1]);
        SIMD_float4 C = s[2].Shuffle<0, 1, 0, 1>(s[3]);
        SIMD_float4 D = s[2].Shuffle<2, 3, 2, 3>(s[3]);

        // determinant as (|A| |B| |C| |D|)
        SIMD_float4 dstsub = s[0].Shuffle<0, 2, 0, 2>(s[2]) * s[1].Shuffle<1, 3, 1, 3>(s[3]) - s[0].Shuffle<1, 3, 1, 3>(s[2]) * s[1].Shuffle<0, 2, 0, 2>(s[3]);

        SIMD_float4 detA = dstsub.Swizzle<0, 0, 0, 0>();
        SIMD_float4 detB = dstsub.Swizzle<1, 1, 1, 1>();
        SIMD_float4 detC = dstsub.Swizzle<2, 2, 2, 2>();
        SIMD_float4 detD = dstsub.Swizzle<3, 3, 3, 3>();

        // let iM = 1/|M| * | X  Y |
        //                  | Z  W |

        // D#C
        SIMD_float4 D_C = D.Swizzle<3, 3, 0, 0>() * C - D.Swizzle<1, 1, 2, 2>() * C.Swizzle<2, 3, 0, 1>();
        // A#B
        SIMD_float4 A_B = A.Swizzle<3, 3, 0, 0>() * B - A.Swizzle<1, 1, 2, 2>() * B.Swizzle<2, 3, 0, 1>();
        // X# = |D|A - B(D#C)
        SIMD_float4 X_ = (detD * A) - (B * D_C.Swizzle<0, 3, 0, 3>() + B.Swizzle<1, 0, 3, 2>() * D_C.Swizzle<2, 1, 2, 1>());
        // W# = |A|D - C(A#B)
        SIMD_float4 W_ = (detA * D) - (C * A_B.Swizzle<0, 3, 0, 3>() + C.Swizzle<1, 0, 3, 2>() * A_B.Swizzle<2, 1, 2, 1>());

        // |M| = |A|*|D| + ... (continue later)
        SIMD_float4 detM = detA * detD;

        // Y# = |B|C - D(A#B)#
        SIMD_float4 Y_ = (detB * C) - (D * A_B.Swizzle<3, 0, 3, 0>() - D.Swizzle<1, 0, 3, 2>() * A_B.Swizzle<2, 1, 2, 1>());
        // Z# = |C|B - A(D#C)#
        SIMD_float4 Z_ = (detC * B) - (A * D_C.Swizzle<3, 0, 3, 0>() - A.Swizzle<1, 0, 3, 2>() * D_C.Swizzle<2, 1, 2, 1>());

        // |M| = |A|*|D| + |B|*|C| ... (continue later)
        detM += detB * detC;

        // tr((A#B)(D#C))
        SIMD_float4 tr = A_B * D_C.Swizzle<3, 1, 2, 0>();
        tr = _mm_hadd_ps(tr, tr);
        tr = _mm_hadd_ps(tr, tr);
        // |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C)
        detM -= tr;

        const SIMD_float4 adjSignMask = SIMD_float4(1.0f, -1.0f, -1.0f, 1.0f);
        // (1/|M|, -1/|M|, -1/|M|, 1/|M|)
        SIMD_float4 rDetM = adjSignMask / detM;

        X_ *= rDetM;
        Y_ *= rDetM;
        Z_ *= rDetM;
        W_ *= rDetM;

        // apply adjugate and store, here we combine adjugate shuffle and store shuffle
        Matrix m;
        {
          m.s[0] = X_.Shuffle<3, 1, 3, 1>(Y_);
          m.s[1] = X_.Shuffle<2, 0, 2, 0>(Y_);
          m.s[2] = Z_.Shuffle<3, 1, 3, 1>(W_);
          m.s[3] = Z_.Shuffle<2, 0, 2, 0>(W_);
        }

        return m;
      }
#endif
      Matrix m;
      {
        m._11 = _22 * _33 * _44 + _23 * _34 * _42 + _24 * _32 * _43 - _24 * _33 * _42 - _23 * _32 * _44 - _22 * _34 * _43;
        m._12 = -_12 * _33 * _44 - _13 * _34 * _42 - _14 * _32 * _43 + _14 * _33 * _42 + _13 * _32 * _44 + _12 * _34 * _43;
        m._13 = _12 * _23 * _44 + _13 * _24 * _42 + _14 * _22 * _43 - _14 * _23 * _42 - _13 * _22 * _44 - _12 * _24 * _43;
        m._14 = -_12 * _23 * _34 - _13 * _24 * _32 - _14 * _22 * _33 + _14 * _23 * _32 + _13 * _22 * _34 + _12 * _24 * _33;

        m._21 = -_21 * _33 * _44 - _23 * _34 * _41 - _24 * _31 * _43 + _24 * _33 * _41 + _23 * _31 * _44 + _21 * _34 * _43;
        m._22 = _11 * _33 * _44 + _13 * _34 * _41 + _14 * _31 * _43 - _14 * _33 * _41 - _13 * _31 * _44 - _11 * _34 * _43;
        m._23 = -_11 * _23 * _44 - _13 * _24 * _41 - _14 * _21 * _43 + _14 * _23 * _41 + _13 * _21 * _44 + _11 * _24 * _43;
        m._24 = _11 * _23 * _34 + _13 * _24 * _31 + _14 * _21 * _33 - _14 * _23 * _31 - _13 * _21 * _34 - _11 * _24 * _33;

        m._31 = _21 * _32 * _44 + _22 * _34 * _41 + _24 * _31 * _42 - _24 * _32 * _41 - _22 * _31 * _44 - _21 * _34 * _42;
        m._32 = -_11 * _32 * _44 - _12 * _34 * _41 - _14 * _31 * _42 + _14 * _32 * _41 + _12 * _31 * _44 + _11 * _34 * _42;
        m._33 = _11 * _22 * _44 + _12 * _24 * _41 + _14 * _21 * _42 - _14 * _22 * _41 - _12 * _21 * _44 - _11 * _24 * _42;
        m._34 = -_11 * _22 * _34 - _12 * _24 * _31 - _14 * _21 * _32 + _14 * _22 * _31 + _12 * _21 * _34 + _11 * _24 * _32;

        m._41 = -_21 * _32 * _43 - _22 * _33 * _41 - _23 * _31 * _42 + _23 * _32 * _41 + _22 * _31 * _43 + _21 * _33 * _42;
        m._42 = _11 * _32 * _43 + _12 * _33 * _41 + _13 * _31 * _42 - _13 * _32 * _41 - _12 * _31 * _43 - _11 * _33 * _42;
        m._43 = -_11 * _22 * _43 - _12 * _23 * _41 - _13 * _21 * _42 + _13 * _22 * _41 + _12 * _21 * _43 + _11 * _23 * _42;
        m._44 = _11 * _22 * _33 + _12 * _23 * _31 + _13 * _21 * _32 - _13 * _22 * _31 - _12 * _21 * _33 - _11 * _23 * _32;
      }

      if (float dst = m._11 * _11 + m._12 * _21 + m._13 * _31 + m._14 * _41;
        dst != 0.0f)
      {
        dst = 1.0f / dst;
        m.r[0] *= dst;
        m.r[1] *= dst;
        m.r[2] *= dst;
        m.r[3] *= dst;
      }

      return m;
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

    [[nodiscard]] static constexpr Matrix RotateX(const Radian& _Angle)noexcept
    {
      const float Cos = Math::Cos(_Angle);
      const float Sin = Math::Sin(_Angle);

      return {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, Cos, Sin, 0.0f,
        0.0f, -Sin, Cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      };
    }

    [[nodiscard]] static constexpr Matrix RotateY(const Radian& _Angle)noexcept
    {
      const float Cos = Math::Cos(_Angle);
      const float Sin = Math::Sin(_Angle);

      return { Cos, 0.0f, -Sin, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        Sin, 0.0f, Cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      };
    }

    [[nodiscard]] static constexpr Matrix RotateZ(const Radian& _Angle)noexcept
    {
      const float Cos = Math::Cos(_Angle);
      const float Sin = Math::Sin(_Angle);

      return { Cos, Sin, 0.0f, 0.0f,
        -Sin, Cos, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };
    }

    [[nodiscard]] static constexpr Matrix Rotate(const Radian& _Pitch, const Radian& _Yaw, const Radian& _Roll)noexcept
    {
      return RotateZ(_Roll) * RotateY(_Yaw) * RotateX(_Pitch);
    }

    [[nodiscard]] static constexpr Matrix Rotate(const Quaternion& _Orientation)noexcept
    {
#if !defined _DEBUG &&__cpp_lib_is_constant_evaluated
      if (!std::is_constant_evaluated())
      {
        return {
          SIMD_float4(1.0f, 0.0f, 0.0f, 0.0f) + SIMD_float4(-2.0f, 2.0f, 2.0f, 0.0f) * (_Orientation.s.Swizzle<1, 0, 0, 0>() * _Orientation.s.Swizzle<1, 1, 2, 0>()
            + _Orientation.s.Swizzle<2, 3, 3, 0>() * _Orientation.s.Swizzle<2, 2, 1, 0>() * SIMD_float4(1.0f, 1.0f, -1.0f, 0.0f)),
          SIMD_float4(0.0f, 1.0f, 0.0f, 0.0f) + SIMD_float4(2.0f, 2.0f, 2.0f, 0.0f) * (_Orientation.s.Swizzle<0, 0, 1, 0>() * _Orientation.s.Swizzle<1, 0, 2, 0>()
            + _Orientation.s.Swizzle<3, 2, 3, 0>() * _Orientation.s.Swizzle<2, 2, 0, 0>() * SIMD_float4(-1.0f, 1.0f, 1.0f, 0.0f)),
          SIMD_float4(0.0f, 0.0f, 1.0f, 0.0f) + SIMD_float4(2.0f, 2.0f, -2.0f, 0.0f) * (_Orientation.s.Swizzle<0, 1, 0, 0>() * _Orientation.s.Swizzle<2, 2, 0, 0>()
            + _Orientation.s.Swizzle<3, 3, 1, 0>() * _Orientation.s.Swizzle<1, 0, 1, 0>() * SIMD_float4(1.0f, -1.0f, 1.0f, 0.0f)),
          SIMD_float4(0.0f, 0.0f, 0.0f, 1.0f)
        };
      }
#endif
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
        2.0f * (xy - wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx), 0.0f,
        2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (xx + yy), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      };
    }
  };

  [[nodiscard]] inline constexpr float4 operator*(const float4& _v, const Matrix& _m)
  {
#if !defined _DEBUG && defined __cpp_lib_is_constant_evaluated
    if (!std::is_constant_evaluated())
    {
      SIMD_float4 x, y, z, w;
      w = _v;

      x = w.Swizzle<0, 0, 0, 0>();
      y = w.Swizzle<1, 1, 1, 1>();
      z = w.Swizzle<2, 2, 2, 2>();
      w = w.Swizzle<3, 3, 3, 3>();

      x *= _m.s[0];
      y *= _m.s[1];
      z *= _m.s[2];
      w *= _m.s[3];

      return x + y + z + w;
    }
#endif
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
    return to_string(_m.r[0]) + ',' + to_string(_m.r[1]) + ',' + to_string(_m.r[2]) + ',' + to_string(_m.r[3]);
  }

  [[nodiscard]] inline wstring to_wstring(const vdl::Matrix& _m)
  {
    return to_wstring(_m.r[0]) + L',' + to_wstring(_m.r[1]) + L',' + to_wstring(_m.r[2]) + L',' + to_wstring(_m.r[3]);
  }

  template <class CharType>
  inline basic_ostream<CharType>& operator<<(basic_ostream<CharType>& _OStream, const vdl::Matrix& _m)
  {
    return _OStream << _m.r[0] << CharType(',') << _m.r[1] << CharType(',') << _m.r[2] << CharType(',') << _m.r[3];
  }

  template <class CharType>
  inline basic_istream<CharType>& operator>>(basic_istream<CharType>& _IStream, vdl::Matrix& _m)
  {
    CharType Temp;

    return _IStream >> _m.r[0] >> Temp >> _m.r[1] >> Temp >> _m.r[2] >> Temp >> _m.r[3];
  }
}
