#include "Mesh.hlsli"

Texture2D Diffuse : register(t0);
Texture2D NormalMap : register(t1);
SamplerState Sampler : register(s0);

float4 main(PS_IN In) : SV_TARGET
{
  return Diffuse.Sample(Sampler, In.Texcoord) * In.Color;
}
