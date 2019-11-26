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

static const float kMaxReflectionRayMarchStep = 1.0f;
static const uint kMaxRayMarchSampleNum = 50;
static const float kReflectionRayMarchStep = kMaxReflectionRayMarchStep / kMaxRayMarchSampleNum;
static const float kEdge = 0.05f;
static const float kRefraction = 1.000293f / 1.333f;

bool Refraction(int3 _Texcoord, float _Depth, out float4 _DiffuseColor, out float3 _ShadowColor)
{
  float3 ScreenSpacePosition = float3(_Texcoord.xy / kWindowSize, WaterSurfaceDepthBuffer.Load(_Texcoord).r);
  float4 Position = GetWorldPosition(ScreenSpacePosition, CameraConstantData.InverseViewProjection);
  float3 Normal = WaterSurfaceNormalGBuffer.Load(_Texcoord).rgb;
  //float3 Normal = float3(0.0f, 1.0f, 0.0f);

  float3 CameraVector = normalize(Position.xyz - CameraConstantData.EyePosition);
  float3 RefractVector = refract(CameraVector, Normal, kRefraction);
       
  float4 PointAlongRefractionVector = float4(Position.xyz + 10.0f * RefractVector, 1.0f);
  float4 ScreenSpaceRefractionPoint = mul(PointAlongRefractionVector, CameraConstantData.ViewProjection);
  ScreenSpaceRefractionPoint /= ScreenSpaceRefractionPoint.w;
  ScreenSpaceRefractionPoint.xy = float2(ScreenSpaceRefractionPoint.x, -ScreenSpaceRefractionPoint.y) * 0.5f + 0.5f;
               
  float3 ScreenSpaceRefractionVector = normalize(ScreenSpaceRefractionPoint.xyz - ScreenSpacePosition);

  for (uint i = 1; i < kMaxRayMarchSampleNum; ++i)
  {
    float3 RayPosition = (i * kReflectionRayMarchStep) * ScreenSpaceRefractionVector + ScreenSpacePosition;

    //  ‰æ–ÊŠO
    if (RayPosition.x < kEdge || RayPosition.y < kEdge || RayPosition.x > (1.0f - kEdge) || RayPosition.y > (1.0f - kEdge) || RayPosition.z >= 1.0f)
    {
      continue;
    }

    float ZBuffer = DepthBuffer.Sample(Sampler, RayPosition.xy).x;
    if (ZBuffer <= _Depth && ZBuffer <= RayPosition.z)
    {
      _DiffuseColor = DiffuseGBuffer.Sample(ClampSampler, RayPosition.xy);
      _DiffuseColor.rgb *= Calc(LightConstantData.DLight, NormalGBuffer.Sample(ClampSampler, RayPosition.xy).xyz);
      _ShadowColor = GetShadowColor(ShadowMap, ShadowSampler, GetWorldPosition(float3(RayPosition.xy, ZBuffer), CameraConstantData.InverseViewProjection),
            LightConstantData.ViewProjection, ShadowConstantData.Color, ShadowConstantData.Bias);
      return true;
    }
  }

  return false;
}

float4 main(float4 _Position : SV_POSITION) : SV_TARGET
{
  int3 Texcoord = int3(_Position.xy, 0);
       
  float3 ScreenSpacePosition = float3(_Position.xy / kWindowSize, DepthBuffer.Load(Texcoord).r);

  float4 DiffuseColor;
  float3 ShadowColor;
  if (!Refraction(Texcoord, ScreenSpacePosition.z, DiffuseColor, ShadowColor))
  {
    float4 Position = GetWorldPosition(ScreenSpacePosition, CameraConstantData.InverseViewProjection);
  
    DiffuseColor = DiffuseGBuffer.Load(Texcoord);
    float3 Normal = NormalGBuffer.Load(Texcoord).xyz;
    DiffuseColor.rgb *= Calc(LightConstantData.DLight, Normal);

    ShadowColor = GetShadowColor(ShadowMap, ShadowSampler, Position, LightConstantData.ViewProjection, ShadowConstantData.Color, ShadowConstantData.Bias);
  }

  return float4(DiffuseColor.rgb * ShadowColor, DiffuseColor.a);
}
