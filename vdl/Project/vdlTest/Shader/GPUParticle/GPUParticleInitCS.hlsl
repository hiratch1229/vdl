#include "GPUParticle.hlsli"

RWStructuredBuffer<int4> Particles : register(u0);

[numthreads(kThreadNum.x, kThreadNum.y, kThreadNum.z)]
void main(uint3 DID : SV_DispatchThreadID)
{
  const uint Index = DID.x;

  if (Index >= kBufferNum)
  {
    return;
  }
        
  Particles[Index].xyzw = Index;
}
