#include "WaterSurface.hlsli"
#include "../Tessellation.hlsli"
#include "../ConstantBuffers.hlsli"

cbuffer ConstantBuffer : register(b0)
{
  CameraData CameraConstantData;
}

ConstantData CulcQuadConstantData(InputPatch<HS_IN, CONTROL_POINT> IPatch)
{
  ConstantData Out;

  for (uint i = 0; i < CONTROL_POINT; ++i)
  {
    Out.TessFactor[i] = GetTessLevel(distance(CameraConstantData.EyePosition, IPatch[i].Position.xyz));
  }

  Out.InsideTessFactor[0] = (Out.TessFactor[0] + Out.TessFactor[1]) / 2.0f;
  Out.InsideTessFactor[1] = (Out.TessFactor[2] + Out.TessFactor[3]) / 2.0f;

  return Out;
}

[domain(DOMAIN)]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(CONTROL_POINT)]
[patchconstantfunc("CulcQuadConstantData")]
DS_IN main(InputPatch<HS_IN, CONTROL_POINT> IPatch, uint ID : SV_OutputControlPointID)
{
  DS_IN Out;

  Out.Position = IPatch[ID].Position;
  Out.Color = IPatch[ID].Color;
  Out.Texcoord = IPatch[ID].Texcoord;

  return Out;
}
