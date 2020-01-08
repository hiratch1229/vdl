#include "../Option/Light.hlsli"

struct PS_IN
{
  float4 Position : SV_Position;
  float3 World : WORLD;
  float3 Normal : NORMAL;
  float3 Tangent : TANGENT;
  float3 Binormal : BINORMAL;
  float4 Color : COLOR;
  float2 Texcoord : TEXCOORD;
};

#define PI 3.14159265359

float D_GGX(float DotNH, float a)
{
  float a2 = a * a;
  float f = (DotNH * DotNH * (a2 - 1.0f) + 1.0f);

  return a2 / (PI * f * f);
}

float G_SmithGGX(float DotNV, float DotNL, float a)
{
  float a2 = a * a;
  float DotNV2 = DotNV * DotNV;
  float GGXV = DotNV2 / (DotNV + sqrt(a2 + (1.0f - a2) * DotNV2));
  float DotNL2 = DotNL * DotNL;
  float GGXL = DotNL2 / (DotNL + sqrt(a2 + (1.0f - a2) * DotNL2));

  return GGXV * GGXL;
}

float3 F_Schlick(float DotVH, float3 f0)
{
  return f0 + (1.0f - f0) * pow(1.0f - DotVH, 5.0f);
}

float Fd_Lambert()
{
  return 1.0f / PI;
}
