#include "../GBufferPass/GBufferPass.hlsli"

struct PS_OUT
{
  float4 Diffuse : SV_TARGET0;
  float4 Normal : SV_TARGET1;
  float4 Specular : SV_TARGET2;
};

SamplerState Sampler : register(s0);
Texture2D Texture : register(t0);
Texture2D NormalMap : register(t1);
Texture2D SpecularMap : register(t2);

PS_OUT main(VS_OUT In)
{
  PS_OUT Out;
  Out.Diffuse = Texture.Sample(Sampler, In.Texcoord) * In.Color;
  float3 N = NormalMap.Sample(Sampler, In.Texcoord).xyz;
  N = 2 * N - 1.0f;
  Out.Normal = float4(normalize(mul(N, float3x3(In.Tangent, In.Binormal, In.Normal))), 1.0f);
  Out.Specular = SpecularMap.Sample(Sampler, In.Texcoord);

  return Out;
}
