#include "GPUParticle.hlsli"

static const uint kAroundNum = kBufferNum / 100;
static const uint kInterval = 10;

RWStructuredBuffer<Particle> Particles : register(u0);

[numthreads(kThreadNum.x, kThreadNum.y, kThreadNum.z)]
void main(uint3 DID : SV_DispatchThreadID)
{
  const uint Index = DID.x;

  if (Index >= kBufferNum)
  {
    return;
  }

  Particle Particle;
  {
    const float Angle = radians(360.0f / kAroundNum) * (Index % kAroundNum);
       
    Particle.Position = kWindowHalfSize + float2(cos(Angle), sin(Angle)) * ((Index / kAroundNum) + 1) * kInterval;
    Particle.Velocity = float2(0.0f, 0.0f);
    Particle.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
  }

  Particles[Index] = Particle;
}
