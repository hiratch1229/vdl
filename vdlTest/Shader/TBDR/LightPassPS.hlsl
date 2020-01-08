#include "PointLight.hlsli"

Texture2D DiffuseGBuffer : register(t0);
Texture2D NormalGBuffer : register(t1);
Texture2D DepthBuffer : register(t2);
StructuredBuffer<PointLight> PointLights : register(t4);

cbuffer LightData : register(b0)
{
  DirectionalLight DLight;
};

cbuffer Data : register(b1)
{
  float3 Ambient;
  uint PointLightNum;
  row_major float4x4 InverseViewProjection;
};

static const uint2 kWindowSize = uint2(1280, 720);

[earlydepthstencil]
float4 main(float4 _Position : SV_POSITION) : SV_TARGET
{
  int3 Texcoord = int3(_Position.xy, 0);

  float2 P = _Position.xy / kWindowSize;
  P = float2(P.x, 1.0f - P.y) * 2.0f - 1.0f;

  float4 Position = mul(float4(P, DepthBuffer.Load(Texcoord).r, 1.0f), InverseViewProjection);
  Position /= Position.w;

  float4 Diffuse = DiffuseGBuffer.Load(Texcoord);
  float3 AmbientColor = Diffuse.rgb * Ambient.rgb;
  float3 Normal = NormalGBuffer.Load(Texcoord).rgb;
  Diffuse.rgb *= Calc(DLight, Normal);

  float3 LightColor = 0.0f;
  {
    for (int i = 0; i < PointLightNum; ++i)
    {
      LightColor += Calc(PointLights[i], Position.xyz, Normal);

    }
  }
    
  return float4(Diffuse.rgb + LightColor + AmbientColor, Diffuse.a);
}
