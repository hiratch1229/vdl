#pragma once

namespace vdl::Math
{
  //  ��
  constexpr float PI = 3.1415926535897932385f;

  //  2��
  constexpr float TwoPI = PI * 2.0f;

  //  1/2��
  constexpr float HalfPI = PI * 0.5f;

  //  ���W�A��(Radian)��x���@(Degree)�ɕϊ�
  constexpr float ToDegree(float _Radian)
  {
    return _Radian * (180.0f / PI);
  }

  //  �x���@(Degree)�����W�A��(Radian)�ɕϊ�
  constexpr float ToRadian(float _Degree)
  {
    return _Degree * (PI / 180.0f);
  }
}
