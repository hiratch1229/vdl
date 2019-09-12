#pragma once

namespace vdl::Math
{
  //  π
  constexpr float kPI = 3.1415926535897932385f;

  //  2π
  constexpr float kTwoPI = kPI * 2.0f;

  //  1/2π
  constexpr float kHalfPI = kPI * 0.5f;

  //  ラジアン(Radian)を度数法(Degree)に変換
  constexpr float ToDegree(float _Radian)
  {
    return _Radian * (180.0f / kPI);
  }

  //  度数法(Degree)をラジアン(Radian)に変換
  constexpr float ToRadian(float _Degree)
  {
    return _Degree * (kPI / 180.0f);
  }
}
