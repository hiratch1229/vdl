#include "GBufferPass.hlsli"

SamplerState Sampler : register(s0);
Texture2D Texture : register(t0);
Texture2D NormalMap : register(t1);

PS_OUT main(VS_OUT In)
{
  PS_OUT Out;
  Out.Diffuse = Texture.Sample(Sampler, In.Texcoord) * In.Color;
  float3 N = NormalMap.Sample(Sampler, In.Texcoord).xyz;
  N = 2 * N - 1.0f;
  Out.Normal = float4(normalize(mul(N, float3x3(In.Tangent, In.Binormal, In.Normal))), 1.0f);

  return Out;
}
