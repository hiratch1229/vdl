#include "Skybox.hlsli"

TextureCube CubeTexture : register(t2);
SamplerState Sampler : register(s0);

float4 main(PS_IN In) : SV_TARGET
{
  return CubeTexture.Sample(Sampler, In.Texcoord);
}