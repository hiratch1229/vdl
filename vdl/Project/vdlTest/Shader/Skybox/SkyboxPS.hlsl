#include "Skybox.hlsli"

SamplerState Sampler : register(s0);
TextureCube CubeTexture : register(t2);

float4 main(PS_IN In) : SV_TARGET
{
  return CubeTexture.Sample(Sampler, In.Texcoord);
}
