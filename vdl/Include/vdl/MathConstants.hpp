#pragma once

namespace vdl::Math
{
  //  π
  constexpr float PI = 3.1415926535897932385f;

  //  2π
  constexpr float TwoPI = PI * 2.0f;

  //  1/2π
  constexpr float HalfPI = PI * 0.5f;

  //  ラジアン(Radian)を度数法(Degree)に変換
  constexpr float ToDegree(float _Radian)
  {
    return _Radian * (180.0f / PI);
  }

  //  度数法(Degree)をラジアン(Radian)に変換
  constexpr float ToRadian(float _Degree)
  {
    return _Degree * (PI / 180.0f);
  }
}
