#include "LightPass.hlsli"
#include "../Option/Light.hlsli"

Texture2D DiffuseGBuffer : register(t0);
Texture2D NormalGBuffer : register(t1);
Texture2D SpecularGBuffer : register(t2);
Texture2D DepthBuffer : register(t3);
Texture2D ShadowMap : register(t4);
SamplerState ShadowMapSampler : register(s0);

static const uint kPointLightNum = 128;
static const uint2 kWindowSize = uint2(1280, 720);

cbuffer LightData : register(b0)
{
  DirectionalLight DLight;
  PointLight PLights[kPointLightNum];
};

cbuffer Data : register(b1)
{
  float3 EyePosition;
  float SpecularPower;
  float4 Ambient;
  float3 Shadow;
  float ShadowBias;
  row_major float4x4 InverseViewProjection;
};

cbuffer Light : register(b2)
{
  row_major float4x4 LightViewProjection;
};

float4 main(VS_OUT In) : SV_TARGET
{
  int3 TexCoord = int3(In.Position.xy, 0);

  float2 P = In.Position.xy / kWindowSize;
  P = float2(P.x, 1.0f - P.y) * 2.0f - 1.0f;

  float4 Position = mul(float4(P, DepthBuffer.Load(TexCoord).r, 1.0f), InverseViewProjection);
  Position /= Position.w;

  float4 Diffuse = DiffuseGBuffer.Load(TexCoord);
  float3 Normal = NormalGBuffer.Load(TexCoord).rgb;

  float3 LightColor = 0.0f;
  {
    LightColor += Calc(DLight, Normal);
    for (int i = 0; i < kPointLightNum; ++i)
    {
      LightColor += Calc(PLights[i], Position.xyz, Normal);
    }
  }

  float3 H = normalize(-DLight.Direction + (EyePosition - Position.xyz));

  float3 SpecularColor = SpecularGBuffer.Load(TexCoord).rgb;

  float3 AmbientColor = Diffuse.rgb * Ambient.rgb;

  float4 ShadowPosition = mul(Position, LightViewProjection);
  ShadowPosition /= ShadowPosition.w;
  ShadowPosition.xy = float2(ShadowPosition.x, -ShadowPosition.y) * 0.5f + 0.5f;

  float d = ShadowMap.Sample(ShadowMapSampler, ShadowPosition.xy).r;
  float3 ShadowColor = (ShadowPosition.z - d > ShadowBias) ? Shadow : 1.0f;

  return float4((Diffuse.rgb + LightColor + AmbientColor + SpecularColor * Specular(H, Normal, SpecularPower)) * ShadowColor, Diffuse.a);
}
