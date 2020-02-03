#include "Terrain.hlsli"
#include "../Deferred.hlsli"
#include "../ConstantBuffers.hlsli"

struct PS_OUT_TEXCOORD
{
  float4 Texcoord : SV_TARGET0;
};

SamplerState Sampler : register(s0);
Texture2D SandTexture : register(t3);
Texture2D RockTexture : register(t4);
Texture2D SlopeTexture : register(t5);
Texture2D GrassTexture : register(t6);
Texture2D NormalMap : register(t7);
Texture2D HeightMap : register(t8);
cbuffer ConstantBuffer : register(b0)
{
  TerrainData TerrainConstantData;
}

PS_OUT_GBUFFER GBufferPass(PS_IN_COLOR In)
{
  PS_OUT_GBUFFER Out;

  const float3 Normal = NormalMap.Sample(Sampler, In.Texcoord).xyz;
  Out.Normal = float4(normalize(mul(Normal * 2.0f - 1.0f, float3x3(In.Tangent, In.Binormal, In.Normal))), 1.0f);

  const float Height = HeightMap.Sample(Sampler, In.Texcoord).x;

  In.Texcoord *= TerrainConstantData.TextureLoopNum;
    
  const float SandHeightDifference = max(0.0f, Height - TerrainConstantData.SandThreshold);
  const float3 SandColor = SandTexture.Sample(Sampler, In.Texcoord).rgb * clamp(1.0f - SandHeightDifference, 0.0f, 1.0f);

  const float RockHeightDifference = max(0.0f, Height - TerrainConstantData.RockThreshold);
  const float3 RockColor = RockTexture.Sample(Sampler, In.Texcoord).rgb * (clamp(SandHeightDifference, 0.0f, 1.0f) - clamp(RockHeightDifference, 0.0f, 1.0f));
  
  const float SlopeHeightDifference = max(0.0f, Height - TerrainConstantData.SlopeThreshold);
  const float3 SlopeColor = SlopeTexture.Sample(Sampler, In.Texcoord).rgb * (clamp(RockHeightDifference, 0.0f, 1.0f) - clamp(SlopeHeightDifference, 0.0f, 1.0f));
 
  const float3 GrassColor = GrassTexture.Sample(Sampler, In.Texcoord).rgb * clamp(SlopeHeightDifference, 0.0f, 1.0f);

  Out.Diffuse = float4(SandColor + RockColor + SlopeColor + GrassColor, 1.0f);
       
  Out.Depth = float4(In.Position.z, 0.0f, 0.0f, 1.0f);

  return Out;
}

PS_OUT_TEXCOORD HeightMapUpdatePrePass(PS_IN_TEXCOORD In)
{
  PS_OUT_TEXCOORD Out;

  Out.Texcoord = float4(In.Texcoord, 0.0f, 1.0f);

  return Out;
}
