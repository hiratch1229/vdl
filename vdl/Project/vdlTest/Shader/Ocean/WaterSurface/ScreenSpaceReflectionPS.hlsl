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

static const float kMaxReflectionRayMarchStep = 1.0f;
static const uint kMaxRayMarchSampleNum = 50;
static const float kReflectionRayMarchStep = kMaxReflectionRayMarchStep / kMaxRayMarchSampleNum;
//static const float kEdge = 0.05f;
static const float kFadeDistance = 0.5f;
static const float kFadeEdge = 2.5f;

bool Reflection(int3 _Texcoord, inout float4 _DiffuseColor)
{
  float3 ScreenSpacePosition = float3(_Texcoord.xy / kWindowSize, WaterSurfaceDepthBuffer.Load(_Texcoord).r);
  float4 Position = GetWorldPosition(ScreenSpacePosition, CameraConstantData.InverseViewProjection);
  //float3 Normal = WaterSurfaceNormalGBuffer.Load(_Texcoord).rgb;
  float3 Normal = float3(0.0f, 1.0f, 0.0f);

  float3 CameraVector = normalize(Position.xyz - CameraConstantData.EyePosition);
  float3 ReflectionVector = reflect(CameraVector, Normal);

  float4 PointAlongReflectionVector = float4(Position.xyz + 10.0f * ReflectionVector, 1.0f);
  float4 ScreenSpaceReflectionPoint = mul(PointAlongReflectionVector, CameraConstantData.ViewProjection);
  ScreenSpaceReflectionPoint /= ScreenSpaceReflectionPoint.w;
  ScreenSpaceReflectionPoint.xy = float2(ScreenSpaceReflectionPoint.x, -ScreenSpaceReflectionPoint.y) * 0.5f + 0.5f;
                            
  float3 ScreenSpaceReflectionVector = normalize(ScreenSpaceReflectionPoint.xyz - ScreenSpacePosition);

  for (uint i = 1; i < kMaxRayMarchSampleNum; ++i)
  {
    float3 RayPosition = (i * kReflectionRayMarchStep) * ScreenSpaceReflectionVector + ScreenSpacePosition;

    ////  ‰æ–ÊŠO
    //if (RayPosition.x < kEdge || RayPosition.y < kEdge || RayPosition.x > (1.0f - kEdge) || RayPosition.y > (1.0f - kEdge) || RayPosition.z >= 1.0f)
    //{
    //  continue;
    //}

    float ZBuffer = DepthBuffer.Sample(Sampler, RayPosition.xy).x;
    if (ZBuffer <= RayPosition.z)
    {
      float DistanceFactor = length(ScreenSpacePosition.xy - RayPosition.xy) / kFadeDistance;
      float Alpha = saturate(1.0f - (DistanceFactor * DistanceFactor));

      float EdgeFactor = kFadeEdge * length(RayPosition.xy - float2(0.5f, 0.5f));
      float Edge = saturate(1.0f - (EdgeFactor * EdgeFactor));
      Alpha *= Edge;

      _DiffuseColor = _DiffuseColor * (1.0f - Alpha) + DiffuseGBuffer.Sample(ClampSampler, RayPosition.xy) * Alpha;

      return true;
    }
  }

  return false;
}

//  http://casual-effects.blogspot.com/2014/08/screen-space-ray-tracing.html
float4 main(float4 _Position : SV_POSITION) : SV_TARGET
{
  int3 Texcoord = int3(_Position.xy, 0);

  float4 DiffuseColor = WaterSurfaceDiffuseGBuffer.Load(Texcoord);
  Reflection(Texcoord, DiffuseColor);
  float3 Normal = WaterSurfaceNormalGBuffer.Load(Texcoord).xyz;
  DiffuseColor.rgb *= Calc(LightConstantData.DLight, Normal);

  float3 ScreenSpacePosition = float3(_Position.xy / kWindowSize, WaterSurfaceDepthBuffer.Load(Texcoord).r);
  float4 Position = GetWorldPosition(ScreenSpacePosition, CameraConstantData.InverseViewProjection);
  float3 ShadowColor = GetShadowColor(ShadowMap, ShadowSampler, Position, LightConstantData.ViewProjection, ShadowConstantData.Color, ShadowConstantData.Bias);

  return float4(DiffuseColor.rgb * ShadowColor, DiffuseColor.a);
}
