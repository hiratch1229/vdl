#include "WaterSurface.hlsli"
#include "../Tessellation.hlsli"
#include "../ConstantBuffers.hlsli"

cbuffer ConstantBuffer : register(b0)
{
  CameraData CameraConstantData;
}
cbuffer ConstantBuffer : register(b2)
{
  WaveData WaveConstantData;
}
cbuffer ConstantBuffer : register(b3)
{
  WaterSurfaceData WaterSurfaceConstantData;
}

//  https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch01.html
[domain(DOMAIN)]
PS_IN main(ConstantData Input, float2 UV : SV_DomainLocation, const OutputPatch<DS_IN, CONTROL_POINT> OPatch)
{
  PS_IN Out;

  Out.Position = lerp(lerp(OPatch[1].Position, OPatch[0].Position, UV.x), lerp(OPatch[3].Position, OPatch[2].Position, UV.x), UV.y);
  Out.Color = lerp(lerp(OPatch[1].Color, OPatch[0].Color, UV.x), lerp(OPatch[3].Color, OPatch[2].Color, UV.x), UV.y);
  Out.Texcoord = lerp(lerp(OPatch[1].Texcoord, OPatch[0].Texcoord, UV.x), lerp(OPatch[3].Texcoord, OPatch[2].Texcoord, UV.x), UV.y);

  float3 ResultPosition = float3(0.0f, 0.0f, 0.0f);
  float3 ResultNormal = float3(0.0f, 0.0f, 0.0f);
  float3 ResultTangent = float3(0.0f, 0.0f, 0.0f);
  float3 ResultBinormal = float3(0.0f, 0.0f, 0.0f);
  for (uint i = 0; i < WaterSurfaceConstantData.WaveNum; ++i)
  {
    Wave Wave = WaveConstantData.Waves[i];

    float Wi = 2.0f / Wave.Length;
    float PhaseConstant = Wave.Speed * Wi;
    float Qi = Wave.Steepness / (Wave.Amplitude * Wi * WaterSurfaceConstantData.WaveNum);
  
    float Radian = Wi * dot(Wave.Direction.xz, Out.Position.xz) + WaterSurfaceConstantData.Time * PhaseConstant;
    float3 Position;
    Position.xz = Out.Position.xz + Qi * Wave.Amplitude * Wave.Direction.xz * cos(Radian);
    Position.y = Wave.Amplitude * sin(Radian);
  
    ResultPosition += Position;
   
    float WA = Wi * Wave.Amplitude;
    Radian = Wi * dot(Wave.Direction, Position) + WaterSurfaceConstantData.Time * PhaseConstant;
    float Cos = cos(Radian);
    float Sin = sin(Radian);

    float3 Normal;
    Normal.x = -(Wave.Direction.x * WA * Cos);
    Normal.z = -(Wave.Direction.z * WA * Cos);
    Normal.y = 1.0f - (Qi * WA * Sin);
               
    ResultNormal += normalize(Normal);

    float3 Tangent;
    Tangent.x = -(Qi * Wave.Direction.x * Wave.Direction.z * WA * Sin);
    Tangent.z = 1.0f - (Qi * Wave.Direction.z * Wave.Direction.z * WA * Sin);
    Tangent.y = (Wave.Direction.z * WA * Cos);

    ResultTangent += normalize(Tangent);

    float3 Binormal;
    Binormal.x = 1.0f - (Qi * Wave.Direction.x * Wave.Direction.x * WA * Sin);
    Binormal.z = -(Qi * Wave.Direction.x * Wave.Direction.z * WA * Sin);
    Binormal.y = (Wave.Direction.x * WA * Cos);

    ResultBinormal += normalize(Binormal);
  }
  
  ResultPosition.xz -= Out.Position.xz * (WaterSurfaceConstantData.WaveNum - 1);
  ResultPosition.y += Out.Position.y;
  ResultNormal = normalize(ResultNormal);
  ResultTangent = normalize(ResultTangent);
  ResultBinormal = normalize(ResultBinormal);
  
  Out.Position = mul(float4(ResultPosition, 1.0f), CameraConstantData.ViewProjection);
  Out.Normal = ResultNormal;
  Out.Tangent = ResultTangent;
  Out.Binormal = ResultBinormal;

  return Out;
}
