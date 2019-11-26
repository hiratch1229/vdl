struct HS_IN
{
  float4 Position : SV_POSITION;
  float4 Color : COLOR;
  float2 Texcoord : TEXCOORD;
};

struct DS_IN
{
  float4 Position : SV_POSITION;
  float3 Normal : NORMAL;
  float4 Color : COLOR;
  float2 Texcoord : TEXCOORD;
};

struct PS_IN
{
  float4 Position : SV_POSITION;
  float3 Normal : NORMAL;
  float3 Tangent : TANGENT;
  float3 Binormal : BINORMAL;
  float4 Color : COLOR;
  float2 Texcoord : TEXCOORD;
};
