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
TextureCube Skybox : register(t7);
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

static const float kFresnelReflectivity = 0.02f;
//static const float kLuminaneThreshold = 0.5f;
//static const float kExposure = 5.0f;

float Fresnel(float f0, float f90, float3 l, float3 n)
{
  return f0 + (f90 - f0) * pow(1.0 - dot(l, n), 5.0);
}

bool Reflection(int3 _Texcoord, inout float4 _DiffuseColor)
{
  const float3 ScreenSpacePosition = float3(_Texcoord.xy / kWindowSize, WaterSurfaceDepthBuffer.Load(_Texcoord).r);
  const float4 Position = GetWorldPosition(ScreenSpacePosition, CameraConstantData.InverseViewProjection);
  //const float3 Normal = WaterSurfaceNormalGBuffer.Load(_Texcoord).rgb;
  const float3 Normal = float3(0.0f, 1.0f, 0.0f);

  const float3 CameraVector = normalize(Position.xyz - CameraConstantData.EyePosition);
  const float3 ReflectionVector = reflect(CameraVector, Normal);

  const float3 ScreenSpaceReflectionVector = GetScreenSpaceReflectionVector(ScreenSpacePosition, Position.xyz, ReflectionVector, CameraConstantData.ViewProjection) * RayMarchConstantData.Step;
  
  _DiffuseColor = lerp(_DiffuseColor, Skybox.Sample(Sampler, ReflectionVector), 0.25f);
  float3 RayPosition;
  float ZBuffer;
  if (!ScreenSpaceRayMarch(DepthBuffer, ClampSampler, ScreenSpacePosition, ScreenSpaceReflectionVector, RayMarchConstantData.SampleNum, RayPosition, ZBuffer))
  {
    return false;
  }
  //BinarySearch(DepthBuffer, ClampSampler, ScreenSpaceReflectionVector, RayPosition, ZBuffer);
  
  float Alpha = DistanceFead(ScreenSpacePosition.xy, RayPosition.xy) * EdgeFead(RayPosition.xy);
  Alpha *= Fresnel(kFresnelReflectivity, 1.0f, CameraConstantData.ViewVector, Normal);
      
  _DiffuseColor = _DiffuseColor * (1.0f - Alpha) + DiffuseGBuffer.Sample(ClampSampler, RayPosition.xy) * Alpha;
    
  return true;
}

float4 main(float4 _Position : SV_POSITION) : SV_TARGET
{
  int3 Texcoord = int3(_Position.xy, 0);
  float3 ScreenSpacePosition = float3(_Position.xy / kWindowSize, WaterSurfaceDepthBuffer.Load(Texcoord).r);

  float4 DiffuseColor = WaterSurfaceDiffuseGBuffer.Load(Texcoord);
  if (ScreenSpacePosition.z < 1.0f)
  {
    Reflection(Texcoord, DiffuseColor);
  }
  float3 Normal = WaterSurfaceNormalGBuffer.Load(Texcoord).xyz;
  DiffuseColor.rgb *= Calc(LightConstantData.DLight, Normal);

  float4 Position = GetWorldPosition(ScreenSpacePosition, CameraConstantData.InverseViewProjection);
  float3 ShadowColor = GetShadowColor(ShadowMap, ShadowSampler, Position, LightConstantData.ViewProjection, ShadowConstantData.Color, ShadowConstantData.Bias);

  return float4(DiffuseColor.rgb * ShadowColor, DiffuseColor.a);
}
