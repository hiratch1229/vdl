#include "WaterSurface.hlsli"
#include "../Tessellation.hlsli"

cbuffer ConstantBuffer : register(b0)
{
  float TessFactor;
  float InsideFactor;
}

ConstantData CulcQuadConstantData(InputPatch<HS_IN, CONTROL_POINT> IPatch)
{
  ConstantData Out;

  Out.TessFactor[0] = Out.TessFactor[1] = Out.TessFactor[2] = Out.TessFactor[3] = TessFactor.x;
  Out.InsideTessFactor[0] = Out.InsideTessFactor[1] = InsideFactor;

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
