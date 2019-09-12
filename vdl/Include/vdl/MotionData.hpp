#pragma once
#include "Types.hpp"

#include <vector>

namespace vdl
{
  //  モーション情報
  struct MotionData
  {
    float Frame;  //  経過フレーム
    uint Number;  //  モーション番号
  };

  struct MotionBlendData : public MotionData
  {
    float BlendRate; // 合成割合
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
