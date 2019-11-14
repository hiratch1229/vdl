#include "WaterSurface.hlsli"
#include "../Tessellation.hlsli"

cbuffer ConstantBuffer : register(b0)
{
  row_major float4x4 ViewProjection;
}
cbuffer ConstantBuffer : register(b1)
{
  Wave Waves[kMaxWaveNum];
}
cbuffer ConstantBuffer : register(b2)
{
  float Time;
  uint WaveNum;
}

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
  for (uint i = 0; i < WaveNum; ++i)
  {
    float Wi = 2.0f / Waves[i].Length;
    float PhaseConstant = Waves[i].Speed * Wi;
    float Qi = Waves[i].Steepness / (Waves[i].Amplitude * Wi * WaveNum);
  
    float Radian = Wi * dot(Waves[i].Direction.xz, Out.Position.xz) + Time * PhaseConstant;
    float3 Position;
    Position.xz = Out.Position.xz + Qi * Waves[i].Amplitude * Waves[i].Direction.xz * cos(Radian);
    Position.y = Waves[i].Amplitude * sin(Radian);
  
    ResultPosition += Position;
   
    float WA = Wi * Waves[i].Amplitude;
    Radian = Wi * dot(Waves[i].Direction, Position) + Time * PhaseConstant;
    float Cos = cos(Radian);
    float Sin = sin(Radian);

    float3 Normal;
    Normal.x = -(Waves[i].Direction.x * WA * Cos);
    Normal.z = -(Waves[i].Direction.z * WA * Cos);
    Normal.y = 1.0f - (Qi * WA * Sin);
               
    ResultNormal += normalize(Normal);

    float3 Tangent;
    Tangent.x = -(Qi * Waves[i].Direction.x * Waves[i].Direction.z * WA * Sin);
    Tangent.z = 1.0f - (Qi * Waves[i].Direction.z * Waves[i].Direction.z * WA * Sin);
    Tangent.y = (Waves[i].Direction.z * WA * Cos);

    ResultTangent += normalize(Tangent);

    float3 Binormal;
    Binormal.x = 1.0f - (Qi * Waves[i].Direction.x * Waves[i].Direction.x * WA * Sin);
    Binormal.z = -(Qi * Waves[i].Direction.x * Waves[i].Direction.z * WA * Sin);
    Binormal.y = (Waves[i].Direction.x * WA * Cos);

    ResultBinormal += normalize(Binormal);
  }
  
  ResultPosition.xz -= Out.Position.xz * (WaveNum - 1);
  ResultPosition.y += Out.Position.y;
  ResultNormal = normalize(ResultNormal);
  ResultTangent = normalize(ResultTangent);
  ResultBinormal = normalize(ResultBinormal);
  
  Out.Position = mul(float4(ResultPosition, 1.0f), ViewProjection);
  Out.Normal = ResultNormal;
  Out.Tangent = ResultTangent;
  Out.Binormal = ResultBinormal;

  return Out;
}
