#include "Mesh.hlsli"

SamplerState Sampler : register(s0);
Texture2D Texture : register(t0);
Texture2D NormalMap : register(t1);

float4 main(PS_IN In) : SV_TARGET
{
  return Texture.Sample(Sampler, In.Texcoord) * In.Color;
}
