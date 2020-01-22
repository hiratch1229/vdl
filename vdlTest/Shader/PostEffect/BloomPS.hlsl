#include "../Texture/Texture.hlsli"

SamplerState Sampler : register(s0);
SamplerState ClampSampler : register(s1);
Texture2D Texture : register(t0);
Texture2D ShrinkBuffer0 : register(t1);
Texture2D ShrinkBuffer1 : register(t2);
Texture2D ShrinkBuffer2 : register(t3);
Texture2D ShrinkBuffer3 : register(t4);

float4 main(PS_IN In) : SV_TARGET
{
  float4 Color = Texture.Sample(ClampSampler, In.Texcoord);

  Color += ShrinkBuffer0.Sample(ClampSampler, In.Texcoord);
  Color += ShrinkBuffer1.Sample(ClampSampler, In.Texcoord);
  Color += ShrinkBuffer2.Sample(ClampSampler, In.Texcoord);
  Color += ShrinkBuffer3.Sample(ClampSampler, In.Texcoord);

  Color.a = 1.0f;

  return Color;
}
