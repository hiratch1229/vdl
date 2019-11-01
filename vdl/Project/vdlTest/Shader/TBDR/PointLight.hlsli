#include "../Option/Light.hlsli"

struct Data
{
  float4 Color;
  float3 MinRange;
  float Timer;
  float3 MaxRange;
  float Time;
};

static const uint kMaxPointLightNum = 4096;
