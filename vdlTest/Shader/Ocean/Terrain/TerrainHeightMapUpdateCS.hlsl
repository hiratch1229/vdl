#include "../ConstantBuffers.hlsli"

Texture2D TexcoordMap : register(t0);
RWTexture2D<float> HeightMap : register(u0);
cbuffer ConstantBuffer : register(b0)
{
  TerrainUpdateData TerrainUpdateConstantData;
}

static const uint2 kThreadGroupNum = uint2(32, 32);
[numthreads(kThreadGroupNum.x, kThreadGroupNum.y, 1)]
void main(uint3 DispatchThreadID : SV_DispatchThreadID)
{
  const float2 Texcoord = TexcoordMap[TerrainUpdateConstantData.MousePosition].xy;

  uint2 HeightMapSize;
  HeightMap.GetDimensions(HeightMapSize.x, HeightMapSize.y);
  
  const float BlushRadius = TerrainUpdateConstantData.BlushSize * 0.5f;
  const float2 Offset = BlushRadius - DispatchThreadID.xy;
  const int2 HeightMapTexcoord = Texcoord * HeightMapSize + Offset;
  
  float Power = TerrainUpdateConstantData.BlushHardness;
  Power *= ((Texcoord.x < 0.0f || Texcoord.y < 0.0f) ? 0.0f : 1.0f);
  Power *= ((Offset.x < -BlushRadius || Offset.y < -BlushRadius || BlushRadius < Offset.x || BlushRadius < Offset.y) ? 0.0f : 1.0f);
  Power *= ((HeightMapTexcoord.x < 0 || HeightMapTexcoord.y < 0 || HeightMapSize.x <= HeightMapTexcoord.x || HeightMapSize.y <= HeightMapTexcoord.y) ? 0.0f : 1.0f);
  
  HeightMap[HeightMapTexcoord] += smoothstep(0.0f, 1.0f, clamp(1.0f - (length(Offset) / BlushRadius), 0.0f, 1.0f)) * Power;
}
