#include "../Option/Light.hlsli"

struct Data
{
  float4 Color;
  float MinRangeX;
  float MinRangeZ;
  float MaxRangeX;
  float MaxRangeZ;
  float Timer;
  float Time;
  float2 Padding;
};

static const uint kMaxPointLightNum = 1024;
