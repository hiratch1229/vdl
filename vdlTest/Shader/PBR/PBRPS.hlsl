#include "PBR.hlsli"

struct MaterialData
{
  float3 CameraPosition;
  float Reflectance;
  float4 Ambient;
};

cbuffer ConstantBuffer : register(b0)
{
  DirectionalLight DLight;
}
cbuffer ConstantBuffer : register(b1)
{
  MaterialData MaterialConstantData;
}

SamplerState Sampler : register(s0);
Texture2D BaseColorMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D MetallicRoughnessMap : register(t2);
Texture2D AmbientOcclusionMap : register(t3);
Texture2D EmissiveMap : register(t4);

float4 main(PS_IN In) : SV_TARGET
{
  float3 BaseColor = BaseColorMap.Sample(Sampler, In.Texcoord).rgb;
  float2 MetallicRoughness = MetallicRoughnessMap.Sample(Sampler, In.Texcoord).bg;
  MetallicRoughness.y = max(MetallicRoughness.y, 0.0001f);
  float3 Emissive = EmissiveMap.Sample(Sampler, In.Texcoord).rgb;
  float AmbientOcclusion = AmbientOcclusionMap.Sample(Sampler, In.Texcoord).r;
  
  float3 N = normalize(mul(NormalMap.Sample(Sampler, In.Texcoord).xyz * 2.0f - 1.0f, float3x3(In.Tangent, In.Binormal, In.Normal)));
    
  float3 L = normalize(-DLight.Direction);
  float3 V = normalize(MaterialConstantData.CameraPosition - In.World);
  float3 H = normalize(L + V);
  
  float DotNH = saturate(dot(N, H));
  float DotNV = saturate(dot(N, V));
  float DotNL = saturate(dot(N, L));
  float DotLH = saturate(dot(L, H));
  
  float a = MetallicRoughness.y * MetallicRoughness.y;
  float D = D_GGX(DotNH, a);
  float G = G_SmithGGX(DotNV, DotNL, a);

  float3 f0 = 0.16f * MaterialConstantData.Reflectance * MaterialConstantData.Reflectance * (1.0f - MetallicRoughness.x) + BaseColor.rgb * MetallicRoughness.x;
  float3 F = F_Schlick(DotLH, f0);
  
  float3 DiffuseColor = (1.0f - MetallicRoughness.x) * BaseColor.rgb;
  float3 AmbientColor = BaseColor.rgb * MaterialConstantData.Ambient.rgb;
  
  float3 Fr = (D * G * F) / (4.0f * DotNV * DotNL);
  float3 Fd = DiffuseColor * Fd_Lambert();
  
  return float4(float3((Fr + Fd) * DLight.Color.rgb * DotNL * DLight.Itensity + Emissive) + AmbientColor * AmbientOcclusion, 1.0f);
}
