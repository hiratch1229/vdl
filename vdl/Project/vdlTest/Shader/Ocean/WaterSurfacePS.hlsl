#include "WaterSurface.hlsli"
#include "../Option/Light.hlsli"

SamplerState Sampler : register(s0);
Texture2D Texture : register(t0);

cbuffer ConstantBuffer : register(b0)
{
  DirectionalLight DLight;
};

float4 main(PS_IN In) : SV_TARGET
{
  //return float4(In.Normal, 1.0f);
  return Texture.Sample(Sampler, In.Texcoord) * In.Color * float4(Calc(DLight, In.Normal), 1.0f);
}
