#include "../Option/Light.hlsli"

SamplerState Sampler : register(s0);
SamplerState ShadowSampler : register(s1);
Texture2D DiffuseGBuffer : register(t0);
Texture2D NormalGBuffer : register(t1);
Texture2D DepthBuffer : register(t2);
Texture2D ShadowMap : register(t3);

static const uint2 kWindowSize = uint2(1280, 720);

cbuffer LightData : register(b0)
{
  DirectionalLight DLight;
  float4 Ambient;
};

cbuffer Data : register(b1)
{
  float3 Shadow;
  float ShadowBias;
  row_major float4x4 InverseViewProjection;
};

cbuffer Data : register(b2)
{
  row_major float4x4 LightViewProjection;
};

float4 main(float4 _Position : SV_POSITION) : SV_TARGET
{
  int3 TexCoord = int3(_Position.xy, 0);

  float2 P = _Position.xy / kWindowSize;
  P = float2(P.x, 1.0f - P.y) * 2.0f - 1.0f;

  float4 Position = mul(float4(P, DepthBuffer.Load(TexCoord).r, 1.0f), InverseViewProjection);
  Position /= Position.w;

  float4 Diffuse = DiffuseGBuffer.Load(TexCoord);
  float3 AmbientColor = Diffuse.rgb * Ambient.rgb;
  float3 Normal = NormalGBuffer.Load(TexCoord).xyz;
  Diffuse.rgb *= Calc(DLight, Normal);
     
  float4 ShadowPosition = mul(Position, LightViewProjection);
  ShadowPosition /= ShadowPosition.w;
  ShadowPosition.xy = float2(ShadowPosition.x, -ShadowPosition.y) * 0.5f + 0.5f;

  float d = ShadowMap.Sample(ShadowSampler, ShadowPosition.xy).r;
  float3 ShadowColor = (ShadowPosition.z - d > ShadowBias) ? Shadow : 1.0f;

  return float4((Diffuse.rgb + AmbientColor) * ShadowColor, Diffuse.a);
}
