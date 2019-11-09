#include "../Mesh/Mesh.hlsli"

struct PS_OUT
{
  float4 Blue : SV_TARGET0;
  float4 Red : SV_TARGET1;
};

Texture2D Diffuse : register(t0);
SamplerState Sampler : register(s0);

PS_OUT main(PS_IN In)
{
  PS_OUT Out;

  Out.Blue = float4(0.0f, 0.0f, 1.0f, 1.0f);
  Out.Red = float4(1.0f, 0.0f, 0.0f, 1.0f);
  //Out.Color = Texture.Sample(Sampler, In.Texcoord) * In.Color;
  //Out.Position = In.Position;

  return Out;
}
