#include "StaticMesh.hlsli"

struct PS_OUT
{
  float4 Color : SV_TARGET0;
  float4 Position : SV_TARGET1;
};


SamplerState Sampler : register(s0);
Texture2D Texture : register(t0);

PS_OUT main(PS_IN In)
{
  PS_OUT Out;

  Out.Color = Texture.Sample(Sampler, In.Texcoord) * In.Color;
  Out.Position = In.Position;

  return Out;
}
