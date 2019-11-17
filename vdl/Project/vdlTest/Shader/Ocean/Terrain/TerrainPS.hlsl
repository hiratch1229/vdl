#include "Terrain.hlsli"
#include "../Deferred.hlsli"

struct PS_OUT_TEXCOORD
{
  float4 Texcoord : SV_TARGET0;
};

SamplerState Sampler : register(s0);
Texture2D Texture : register(t0);
Texture2D NormalMap : register(t2);

PS_OUT_GBUFFER GBufferPass(PS_IN_COLOR In)
{
  PS_OUT_GBUFFER Out;

  Out.Diffuse = Texture.Sample(Sampler, In.Texcoord) * In.Color;

  float3 Normal = NormalMap.Sample(Sampler, In.Texcoord).xyz * 2.0f - 1.0f;
  Out.Normal = float4(normalize(mul(Normal, float3x3(In.Tangent, In.Binormal, In.Normal))), 1.0f);

  return Out;
}

PS_OUT_TEXCOORD HeightMapUpdatePrePass(PS_IN_TEXCOORD In)
{
  PS_OUT_TEXCOORD Out;

  Out.Texcoord = float4(In.Texcoord, 0.0f, 1.0f);

  return Out;
}
