#include "../Texture/Texture.hlsli"
#include "../Option/GaussianBlur.hlsli"

SamplerState Sampler : register(s0);
SamplerState ClampSampler : register(s1);
Texture2D Texture : register(t0);

float4 VertexBlur(PS_IN In) : SV_TARGET
{
  return float4(VerticalGaussianBlur(Texture, ClampSampler, In.Texcoord), 1.0f);
}

float4 HorizontalBlur(PS_IN In) : SV_TARGET
{
  return float4(HorizontalGaussianBlur(Texture, ClampSampler, In.Texcoord), 1.0f);
}
