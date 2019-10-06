#include "GPUParticle.hlsli"

float4 main(PS_IN In) : SV_TARGET
{
  return float4(In.Color.rgb, In.Color.a * smoothstep(0.0f, 1.0f, clamp((0.5f - length(In.Texcoord - float2(0.5f, 0.5f))) / 0.5f, 0.0f, 1.0f)));
}
