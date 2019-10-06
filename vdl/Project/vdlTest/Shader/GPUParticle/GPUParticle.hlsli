struct Particle
{
  float2 Position;
  float2 Velocity;
  float4 Color;
};

struct GS_IN
{
  float4 Position : SV_POSITION;
  float4 Color : COLOR;
};

struct PS_IN
{
  float4 Position : SV_POSITION;
  float4 Color : COLOR;
  float2 Texcoord : TEXCOORD;
};

static const uint2 kWindowSize = uint2(1280, 720);
static const float2 kWindowHalfSize = kWindowSize * 0.5f;
static const uint kBufferNum = 1000000;
static const uint3 kThreadNum = uint3(512, 1, 1);
