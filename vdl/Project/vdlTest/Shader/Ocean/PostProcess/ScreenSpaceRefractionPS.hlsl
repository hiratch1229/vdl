#include "ScreenSpaceRayMarch.hlsli"
#include "../ConstantBuffers.hlsli"
#include "../PostProcess.hlsli"
#include "../Shadow.hlsli"

SamplerState Sampler : register(s0);
SamplerState ShadowSampler : register(s1);
SamplerState ClampSampler : register(s2);
Texture2D WaterSurfaceDiffuseGBuffer : register(t0);
Texture2D WaterSurfaceNormalGBuffer : register(t1);
Texture2D WaterSurfaceDepthBuffer : register(t2);
Texture2D DiffuseGBuffer : register(t3);
Texture2D NormalGBuffer : register(t4);
Texture2D DepthBuffer : register(t5);
Texture2D ShadowMap : register(t6);
cbuffer ConstantBuffer : register(b0)
{
  CameraData CameraConstantData;
}
cbuffer ConstantBuffer : register(b1)
{
  LightData LightConstantData;
}
cbuffer ConstantBuffer : register(b2)
{
  ShadowData ShadowConstantData;
}
cbuffer ConstantBuffer : register(b3)
{
  RayMarchData RayMarchConstantData;
}

static const float kRefraction = 1.000293f / 1.333f;

bool Refraction(int3 _Texcoord, float _Depth, inout float4 _DiffuseColor, out float3 _ShadowColor)
{
  if (_Depth >= 1.0f)
  {
    return false;
  }
  
  const float3 ScreenSpacePosition = float3(_Texcoord.xy / kWindowSize, WaterSurfaceDepthBuffer.Load(_Texcoord).r);
  const float4 Position = GetWorldPosition(ScreenSpacePosition, CameraConstantData.InverseViewProjection);
  const float3 Normal = WaterSurfaceNormalGBuffer.Load(_Texcoord).rgb;

  const float3 CameraVector = normalize(Position.xyz - CameraConstantData.EyePosition);
  const float3 RefractVector = refract(CameraVector, Normal, kRefraction);
       
  const float3 ScreenSpaceRefractionVector = GetScreenSpaceReflectionVector(ScreenSpacePosition, Position.xyz, RefractVector, CameraConstantData.ViewProjection) * RayMarchConstantData.Step;
    
  float3 RayPosition;
  float ZBuffer;
  if (!ScreenSpaceRayMarch(DepthBuffer, ClampSampler, ScreenSpacePosition, ScreenSpaceRefractionVector, RayMarchConstantData.SampleNum, RayPosition, ZBuffer))
  {
    return false;
  }
  BinarySearch(DepthBuffer, ClampSampler, ScreenSpaceRefractionVector, RayPosition, ZBuffer);
  
  float Alpha = EdgeFead(RayPosition.xy);
                 
  _ShadowColor = saturate(GetShadowColor(ShadowMap, ShadowSampler, GetWorldPosition(float3(RayPosition.xy, ZBuffer), CameraConstantData.InverseViewProjection),
            LightConstantData.ViewProjection, ShadowConstantData.Color, ShadowConstantData.Bias) * (1.0f / Alpha));

  Alpha *= DistanceFead(ScreenSpacePosition.xy, RayPosition.xy);
  float4 DiffuseColor = DiffuseGBuffer.Sample(ClampSampler, RayPosition.xy);
  DiffuseColor.rgb *= Calc(LightConstantData.DLight, NormalGBuffer.Sample(ClampSampler, RayPosition.xy).xyz);
  _DiffuseColor = _DiffuseColor * (1.0f - Alpha) + DiffuseColor * Alpha;
    
  return true;
}

float4 main(float4 _Position : SV_POSITION) : SV_TARGET
{
  int3 Texcoord = int3(_Position.xy, 0);
       
  float3 ScreenSpacePosition = float3(GetNDCPosition(_Position.xy), DepthBuffer.Load(Texcoord).r);

  float4 DiffuseColor = DiffuseGBuffer.Load(Texcoord);
  float3 Normal = NormalGBuffer.Load(Texcoord).xyz;
  DiffuseColor.rgb *= Calc(LightConstantData.DLight, Normal);

  float3 ShadowColor;
  
  if (!Refraction(Texcoord, ScreenSpacePosition.z, DiffuseColor, ShadowColor))
  {
    float4 Position = GetWorldPosition(ScreenSpacePosition, CameraConstantData.InverseViewProjection);
    ShadowColor = GetShadowColor(ShadowMap, ShadowSampler, Position, LightConstantData.ViewProjection, ShadowConstantData.Color, ShadowConstantData.Bias);
  }

  return float4(DiffuseColor.rgb * ShadowColor, DiffuseColor.a);
}
