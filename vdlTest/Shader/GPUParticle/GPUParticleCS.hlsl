#include "GPUParticle.hlsli"

static const float kSpeed = 2.5f;
static const float kAlphaDenominator = 25.0f;

RWStructuredBuffer<Particle> Particles : register(u0);

cbuffer ConstantBuffer : register(b0)
{
  float2 MousePosition;
  float DeltaTime;
};

[numthreads(kThreadNum.x, kThreadNum.y, kThreadNum.z)]
void main(uint3 DID : SV_DispatchThreadID)
{
  const uint Index = DID.x;

  if (Index >= kBufferNum)
  {
    return;
  }

  Particle Particle = Particles[Index];

  Particle.Velocity += normalize(MousePosition - Particle.Position) * DeltaTime * kSpeed;
  Particle.Position += Particle.Velocity;

  const float Length = length(Particle.Velocity);
  Particle.Color.xz = (Particle.Velocity / Length) * 0.5f + 0.5f;
  Particle.Color.y = 0.5f;
  Particle.Color.a = clamp(Length / kAlphaDenominator, 0.0f, 1.0f);

  Particles[Index] = Particle;
}
