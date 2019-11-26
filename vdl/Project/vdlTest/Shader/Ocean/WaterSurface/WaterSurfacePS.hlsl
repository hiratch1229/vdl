#include "WaterSurface.hlsli"
#include "../Deferred.hlsli"
#include "../ConstantBuffers.hlsli"

SamplerState Sampler : register(s0);
Texture2D NormalMap0 : register(t9);
Texture2D NormalMap1 : register(t10);
cbuffer ConstantBuffer : register(b1)
{
  WaterSurfaceData WaterSurfaceConstantData;
}

static const float2 NormalMapScrollSpeed0 = { 0.01f, 0.01f };
static const float2 NormalMapScrollSpeed1 = { -0.005f, 0.0f };

PS_OUT_GBUFFER main(PS_IN In) : SV_TARGET
{
  PS_OUT_GBUFFER Out;

  Out.Diffuse = In.Color;

  float3 Normal0 = (NormalMap0.Sample(Sampler, In.Texcoord + WaterSurfaceConstantData.Time * NormalMapScrollSpeed0).xyz * 2.0f) - 1.0f;
  float3 Normal1 = (NormalMap1.Sample(Sampler, In.Texcoord + WaterSurfaceConstantData.Time * NormalMapScrollSpeed1).xyz * 2.0f) - 1.0f;

  Out.Normal = float4(normalize(mul(normalize(Normal0 + Normal1), float3x3(In.Tangent, In.Binormal, In.Normal))), 1.0f);

  Out.Depth = float4(In.Position.z, 0.0f, 0.0f, 1.0f);

  return Out;
}
