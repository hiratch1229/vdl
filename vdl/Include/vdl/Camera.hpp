#pragma once
#include "Fwd.hpp"
#include "Type3.hpp"
#include "Angle.hpp"

namespace vdl
{
  struct Camera
  {
    //  �ʒu
    float3 Pos = { 0.0f, 0.0f, -10.0f };
    //  �����_
    float3 Target = { 0.0f, 0.0f, 0.0f };
    //  �����
    float3 Up = { 0.0f, 1.0f, 0.0f };
    //  �ŋߖ�
    float Near = 0.1f;
    //  �ŉ���
    float Far = 1000.0f;
    //  ����p
    Degree Fov = 30.0f;
    //  true�œ������e
    bool isPerspective = true;
  };
}
