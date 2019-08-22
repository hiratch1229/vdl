#pragma once
#include "Type2.hpp"
#include "Type3.hpp"
#include "Type4.hpp"
#include "Quaternion.hpp"
#include "Angle.hpp"

#include <DirectXMath.h>

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

    Matrix(const DirectX::XMMATRIX& _m)noexcept
      : _11(_m.r[0].m128_f32[0]), _12(_m.r[0].m128_f32[1]), _13(_m.r[0].m128_f32[2]), _14(_m.r[0].m128_f32[3])
      , _21(_m.r[1].m128_f32[0]), _22(_m.r[1].m128_f32[1]), _23(_m.r[1].m128_f32[2]), _24(_m.r[1].m128_f32[3])
      , _31(_m.r[2].m128_f32[0]), _32(_m.r[2].m128_f32[1]), _33(_m.r[2].m128_f32[2]), _34(_m.r[2].m128_f32[3])
      , _41(_m.r[3].m128_f32[0]), _42(_m.r[3].m128_f32[1]), _43(_m.r[3].m128_f32[2]), _44(_m.r[3].m128_f32[3]) {}

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
    operator DirectX::XMMATRIX()const noexcept { return { _11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44 }; }
  public:
    [[nodiscard]] Matrix operator*(const Matrix& _m)const { return DirectX::XMMatrixMultiply(*this, _m); }

    Matrix& operator=(const Matrix& _m)noexcept
    {
      r[0] = _m.r[0];
      r[1] = _m.r[1];
      r[2] = _m.r[2];
      r[3] = _m.r[3];

      return *this;
    }

    Matrix& operator*=(const Matrix& _m)
    {
      *this = DirectX::XMMatrixMultiply(*this, _m);

      return *this;
    }
  public:
    [[nodiscard]] constexpr float3 Right()const noexcept { return { _11, _12, _13 }; }

    [[nodiscard]] constexpr float3 Left()const noexcept { return { -_11, -_12, -_13 }; }

    [[nodiscard]] constexpr float3 Up()const noexcept { return { _21, _22, _23 }; }

    [[nodiscard]] constexpr float3 Down()const noexcept { return { -_21, -_22, -_23 }; }

    [[nodiscard]] constexpr float3 Forward()const noexcept { return { _31, _32, _33 }; }

    [[nodiscard]] constexpr float3 Back()const noexcept { return { -_31,-_32, -_33 }; }

    [[nodiscard]] constexpr float3 Translation()const noexcept { return { _41, _42, _43 }; }

    [[nodiscard]] Matrix Inverse()const { return DirectX::XMMatrixInverse(nullptr, *this); }

    [[nodiscard]] Matrix Transpose()const { return DirectX::XMMatrixTranspose(*this); }
  public:
    [[nodiscard]] static constexpr Matrix Identity()noexcept
    {
      return Matrix(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    }

    [[nodiscard]] static Matrix Translate(const float3& _Pos)
    {
      return DirectX::XMMatrixTranslation(_Pos.x, _Pos.y, _Pos.z);
    }

    [[nodiscard]] static Matrix Scale(const float3& _Scale)
    {
      return DirectX::XMMatrixScaling(_Scale.x, _Scale.y, _Scale.z);
    }

    [[nodiscard]] static Matrix Rotate(const Radian& _Pitch, const Radian& _Yaw, const Radian& _Roll)
    {
      return DirectX::XMMatrixRotationRollPitchYaw(_Pitch, _Yaw, _Roll);
    }

    [[nodiscard]] static Matrix Rotate(const Quaternion& _Orientation)
    {
      return DirectX::XMMatrixRotationQuaternion(_Orientation);
    }
  };

  [[nodiscard]] inline float2 operator*(const float2& _v, const Matrix& _m)
  {
    const DirectX::XMVECTOR v = DirectX::XMVector2Transform({ _v.x, _v.y, 0.0f, 0.0f }, _m);

    return { v.m128_f32[0], v.m128_f32[1] };
  }

  [[nodiscard]] inline float3 operator*(const float3& _v, const Matrix& _m)
  {
    const DirectX::XMVECTOR v = DirectX::XMVector3Transform({ _v.x, _v.y, _v.z, 0.0f }, _m);

    return { v.m128_f32[0], v.m128_f32[1], v.m128_f32[2] };
  }

  [[nodiscard]] inline float4 operator*(const float4& _v, const Matrix& _m)
  {
    const DirectX::XMVECTOR v = DirectX::XMVector3Transform({ _v.x, _v.y, _v.z, _v.w }, _m);

    return { v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3] };
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
