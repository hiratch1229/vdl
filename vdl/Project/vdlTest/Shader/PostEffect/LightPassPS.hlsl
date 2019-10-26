#include "../Option/Light.hlsli"

Texture2D DiffuseGBuffer : register(t0);
Texture2D NormalGBuffer : register(t1);
Texture2D DepthBuffer : register(t2);
Texture2D ShadowMap : register(t3);
SamplerState ShadowMapSampler : register(s0);

static const uint2 kWindowSize = uint2(1280, 720);
static const float kNear = 25.0f;
static const float kFar = 50.0f;

cbuffer LightData : register(b0)
{
  DirectionalLight DLight;
};

cbuffer Data : register(b1)
{
  float3 EyePos;
  float LuminanceThreshold;
  float3 Fog;
  float Exposure;
  float3 Shadow;
  float ShadowBias;
  row_major float4x4 InverseViewProjection;
};

cbuffer Light : register(b2)
{
  row_major float4x4 LightViewProjection;
};

float GetLuminance(float3 _Color)
{
  return _Color.r * 0.29891f + _Color.g * 0.58661f + _Color.b * 0.11448f;
}

struct PS_OUT
{
  float4 Color : SV_TARGET0;
  float4 Luminance : SV_TARGET1;
};

PS_OUT main(float4 _Position : SV_POSITION)
{
  PS_OUT Out;

  int3 TexCoord = int3(_Position.xy, 0);

  float2 P = _Position.xy / kWindowSize;
  P = float2(P.x, 1.0f - P.y) * 2.0f - 1.0f;

  float4 Position = mul(float4(P, DepthBuffer.Load(TexCoord).r, 1.0f), InverseViewProjection);
  Position /= Position.w;

  float4 Diffuse = DiffuseGBuffer.Load(TexCoord);
  float3 Normal = NormalGBuffer.Load(TexCoord).rgb;

  float3 LightColor = Calc(DLight, Normal);

  float4 ShadowPosition = mul(Position, LightViewProjection);
  ShadowPosition /= ShadowPosition.w;
  ShadowPosition.xy = float2(ShadowPosition.x, -ShadowPosition.y) * 0.5f + 0.5f;

  float d = ShadowMap.Sample(ShadowMapSampler, ShadowPosition.xy).r;
  float3 ShadowColor = (ShadowPosition.z - d > ShadowBias) ? Shadow : 1.0f;

  float Alpha = saturate((length(Position.xyz - EyePos) - kNear) / (kFar - kNear));

  float3 Color = lerp(float3((Diffuse.rgb * LightColor) * ShadowColor), Fog, saturate((length(Position.xyz - EyePos.xyz) - kNear) / (kFar - kNear)));

  Out.Color = float4(Color, Diffuse.a);
  Out.Luminance = float4(Color * max(0.0f, GetLuminance(Color) - LuminanceThreshold) * Exposure, 1.0f);

  return Out;
}
