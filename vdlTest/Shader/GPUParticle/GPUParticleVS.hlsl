#include "GPUParticle.hlsli"

StructuredBuffer<Particle> Particles : register(t0);

PS_IN main(uint Index : SV_VertexID)
{
  PS_IN Out;
       
  Out.Position = float4(Particles[Index].Position, 0.0f, 1.0f);
  Out.Color = Particles[Index].Color;

  return Out;
}
