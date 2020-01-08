#include "Terrain.hlsli"
#include "../Tessellation.hlsli"
#include "../ConstantBuffers.hlsli"

SamplerState Sampler : register(s0);
Texture2D HeightMap : register(t0);
cbuffer ConstantBuffer : register(b0)
{
  CameraData CameraConstantData;
}
cbuffer ConstantBuffer : register(b1)
{
  LightData LightConstantData;
}

float4 CalcPosition(float4 _Position, float3 _Normal, float2 _Texcoord)
{
  _Position.xyz += _Normal * HeightMap.SampleLevel(Sampler, _Texcoord, 0).r;
  return _Position;
}

[domain(DOMAIN)]
PS_IN_COLOR Color(ConstantData Input, float2 UV : SV_DomainLocation, const OutputPatch<DS_IN, CONTROL_POINT> OPatch)
{
  PS_IN_COLOR Out;

  Out.Position = lerp(lerp(OPatch[1].Position, OPatch[0].Position, UV.x), lerp(OPatch[3].Position, OPatch[2].Position, UV.x), UV.y);
  Out.Normal = lerp(lerp(OPatch[1].Normal, OPatch[0].Normal, UV.x), lerp(OPatch[3].Normal, OPatch[2].Normal, UV.x), UV.y);
  Out.Tangent = lerp(lerp(OPatch[1].Tangent, OPatch[0].Tangent, UV.x), lerp(OPatch[3].Tangent, OPatch[2].Tangent, UV.x), UV.y);
  Out.Color = lerp(lerp(OPatch[1].Color, OPatch[0].Color, UV.x), lerp(OPatch[3].Color, OPatch[2].Color, UV.x), UV.y);
  Out.Texcoord = lerp(lerp(OPatch[1].Texcoord, OPatch[0].Texcoord, UV.x), lerp(OPatch[3].Texcoord, OPatch[2].Texcoord, UV.x), UV.y);

  Out.Position = CalcPosition(Out.Position, Out.Normal, Out.Texcoord);
  Out.World = Out.Position.xyz / Out.Position.w;
  Out.Position = mul(Out.Position, CameraConstantData.ViewProjection);
  Out.Binormal = cross(Out.Normal, Out.Tangent);

  return Out;
}

[domain(DOMAIN)]
PS_IN_TEXCOORD Texcoord(ConstantData Input, float2 UV : SV_DomainLocation, const OutputPatch<DS_IN, CONTROL_POINT> OPatch)
{
  PS_IN_TEXCOORD Out;

  Out.Position = lerp(lerp(OPatch[1].Position, OPatch[0].Position, UV.x), lerp(OPatch[3].Position, OPatch[2].Position, UV.x), UV.y);
  Out.Texcoord = lerp(lerp(OPatch[1].Texcoord, OPatch[0].Texcoord, UV.x), lerp(OPatch[3].Texcoord, OPatch[2].Texcoord, UV.x), UV.y);
  float3 Normal = lerp(lerp(OPatch[1].Normal, OPatch[0].Normal, UV.x), lerp(OPatch[3].Normal, OPatch[2].Normal, UV.x), UV.y);

  Out.Position = mul(CalcPosition(Out.Position, Normal, Out.Texcoord), CameraConstantData.ViewProjection);

  return Out;

}

[domain(DOMAIN)]
PS_IN_SHADOW Shadow(ConstantData Input, float2 UV : SV_DomainLocation, const OutputPatch<DS_IN, CONTROL_POINT> OPatch)
{
  PS_IN_SHADOW Out;

  Out.Position = lerp(lerp(OPatch[1].Position, OPatch[0].Position, UV.x), lerp(OPatch[3].Position, OPatch[2].Position, UV.x), UV.y);
  float3 Normal = lerp(lerp(OPatch[1].Normal, OPatch[0].Normal, UV.x), lerp(OPatch[3].Normal, OPatch[2].Normal, UV.x), UV.y);
  float2 Texcoord = lerp(lerp(OPatch[1].Texcoord, OPatch[0].Texcoord, UV.x), lerp(OPatch[3].Texcoord, OPatch[2].Texcoord, UV.x), UV.y);

  Out.Position = mul(CalcPosition(Out.Position, Normal, Texcoord), LightConstantData.ViewProjection);

  return Out;
}
