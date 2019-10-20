struct VS_OUT
{
  float4 Position : SV_POSITION;
  float4 Color : COLOR;
  float2 Texcoord : TEXCOORD;
  float3 Normal : NORMAL;
  float3 Tangent : TANGENT;
  float3 Binormal : BINORMAL;
};

struct PS_OUT
{
  float4 Diffuse : SV_TARGET0;
  float4 Normal : SV_TARGET1;
  float4 Specular : SV_TARGET2;
};
