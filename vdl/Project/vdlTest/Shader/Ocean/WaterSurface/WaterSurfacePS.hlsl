#include "WaterSurface.hlsli"
#include "../../Option/Light.hlsli"

SamplerState Sampler : register(s0);
Texture2D NormalMap0 : register(t9);
Texture2D NormalMap1 : register(t10);

cbuffer ConstantBuffer : register(b0)
{
  DirectionalLight DLight;
  float4 Ambient;
};
cbuffer ConstantBuffer : register(b1)
{
  float Time;
  uint WaveNum;
}

static const float2 NormalMapScrollSpeed0 = { 0.01f, 0.01f };
static const float2 NormalMapScrollSpeed1 = { -0.005f, 0.0f };

float4 main(PS_IN In) : SV_TARGET
{
  //return float4(In.Texcoord.y, 0.0f, 0.0f, 1.0f);

  float3 Normal0 = (NormalMap0.Sample(Sampler, In.Texcoord + Time * NormalMapScrollSpeed0).xyz * 2.0f) - 1.0f;
  float3 Normal1 = (NormalMap1.Sample(Sampler, In.Texcoord + Time * NormalMapScrollSpeed1).xyz * 2.0f) - 1.0f;

  float3 Normal = normalize(mul(normalize(Normal0 + Normal1), float3x3(In.Tangent, In.Binormal, In.Normal)));
  //return float4(Normal, 1.0f);

  return float4(Calc(DLight, Normal) + Ambient.rgb, 1.0f) * In.Color;
}
