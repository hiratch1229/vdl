#include "Tessellation.hlsli"

cbuffer ConstantBuffer : register(b0)
{
  float3 TessFactor;
  float InsideFactor;
}

TriangleConstantData CulcTriangleConstantData(InputPatch<VS_OUT, CONTROL_POINT_TRIANGLE> IPatch)
{
  TriangleConstantData Out;

  Out.TessFactor[0] = TessFactor.x;
  Out.TessFactor[1] = TessFactor.y;
  Out.TessFactor[2] = TessFactor.z;

  Out.InsideTessFactor = InsideFactor;

  return Out;
}

QuadConstantData CulcQuadConstantData(InputPatch<VS_OUT, CONTROL_POINT_QUAD> IPatch)
{
  QuadConstantData Out;

  Out.TessFactor[0] = TessFactor.x;
  Out.TessFactor[1] = TessFactor.y;
  Out.TessFactor[2] = TessFactor.z;
  Out.TessFactor[3] = 64.0f;

  Out.InsideTessFactor[0] = InsideFactor;
  Out.InsideTessFactor[1] = InsideFactor;

  return Out;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(CONTROL_POINT_QUAD)]
[patchconstantfunc("CulcQuadConstantData")]
HS_OUT main(InputPatch<VS_OUT, CONTROL_POINT_QUAD> IPatch, uint ID : SV_OutputControlPointID)
{
  HS_OUT Out;

  Out.Position = IPatch[ID].Position;
  Out.Color = IPatch[ID].Color;

  return Out;
}
