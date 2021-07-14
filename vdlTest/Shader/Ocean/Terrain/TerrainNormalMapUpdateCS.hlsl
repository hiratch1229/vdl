RWTexture2D<float> HeightMap : register(u0);
RWTexture2D<float4> NormalMap : register(u1);

static const uint2 kThreadGroupNum = uint2(32, 32);
static const uint2 kHeightMapSize = uint2(4096, 4096);

[numthreads(kThreadGroupNum.x, kThreadGroupNum.y, 1)]
void main(uint3 DispatchThreadID : SV_DispatchThreadID)
{
  const uint2 Texcoord = DispatchThreadID.xy;

  float3 X = float3(1.0f, 0.0f, HeightMap[Texcoord + int2(+1, 0)] - HeightMap[Texcoord + int2(-1, 0)]);
  float3 Y = float3(0.0f, 1.0f, HeightMap[Texcoord + int2(0, +1)] - HeightMap[Texcoord + int2(0, -1)]);

  NormalMap[Texcoord] = float4((normalize(cross(X, Y)) + 1.0f) / 2.0f, 1.0f);
}
