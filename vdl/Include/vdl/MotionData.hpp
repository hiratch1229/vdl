#pragma once
#include "Types.hpp"

#include <vector>

namespace vdl
{
  //  ���[�V�������
  struct MotionData
  {
    float Frame;  //  �o�߃t���[��
    uint Number;  //  ���[�V�����ԍ�
  };

  struct MotionBlendData : public MotionData
  {
    float BlendRate; // ��������
  public:
    MotionBlendData() = default;

    MotionBlendData(const MotionData& _MotionData, float _BlendRate)
      : BlendRate(_BlendRate)
    {
      Frame = _MotionData.Frame;
      Number = _MotionData.Number;
    }
    MotionBlendData(const MotionData& _MotionData)
    {
      Frame = _MotionData.Frame;
      Number = _MotionData.Number;
      BlendRate = 1.0f;
    }
    const MotionBlendData& operator=(const MotionData& _MotionData)
    {
      Frame = _MotionData.Frame;
      Number = _MotionData.Number;

      return *this;
    }
  };

  using MotionBlendDatas = std::vector<MotionBlendData>;
}
