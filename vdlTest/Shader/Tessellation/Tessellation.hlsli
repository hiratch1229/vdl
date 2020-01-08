struct VS_OUT
{
  float4 Position : SV_POSITION;
  float4 Color : COLOR;
};

struct HS_OUT
{
  float4 Position : SV_POSITION;
  float4 Color : COLOR;
};

struct DS_OUT
{
  float4 Position : SV_POSITION;
  float4 Color : COLOR;
};

struct TriangleConstantData
{
  float TessFactor[3] : SV_TessFactor;
  float InsideTessFactor : SV_InsideTessFactor;
};

struct QuadConstantData
{
  float TessFactor[4] : SV_TessFactor;
  float InsideTessFactor[2] : SV_InsideTessFactor;
};

#define CONTROL_POINT_TRIANGLE 3
#define CONTROL_POINT_QUAD 4
