#pragma once

namespace vdl::Math
{
  //  ��
  constexpr float kPI = 3.1415926535897932385f;

  //  2��
  constexpr float kTwoPI = kPI * 2.0f;

  //  1/2��
  constexpr float kHalfPI = kPI * 0.5f;

  //  ���W�A��(Radian)��x���@(Degree)�ɕϊ�
  constexpr float ToDegree(float _Radian)
  {
    return _Radian * (180.0f / kPI);
  }

  //  �x���@(Degree)�����W�A��(Radian)�ɕϊ�
  constexpr float ToRadian(float _Degree)
  {
    return _Degree * (kPI / 180.0f);
  }
}
