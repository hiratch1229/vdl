#include "../ConstantBuffers.hlsli"

Texture2D TexcoordMap : register(t0);
RWTexture2D<float> HeightMap : register(u0);
cbuffer ConstantBuffer : register(b0)
{
  TerrainUpdateData TerrainUpdateConstantData;
}

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_GroupID)
{
  const float2 Texcoord = TexcoordMap[TerrainUpdateConstantData.MousePosition].xy;
  if (Texcoord.x < 0.0f || Texcoord.y < 0.0f)
  {
    return;
  }

  uint2 HeightMapSize;
  HeightMap.GetDimensions(HeightMapSize.x, HeightMapSize.y);
  
  const float BlushRadius = TerrainUpdateConstantData.BlushSize * 0.5f;
  const float2 Offset = BlushRadius - DTid.xy;
  const int2 HeightMapTexcoord = Texcoord * HeightMapSize + Offset;
  
  if (HeightMapTexcoord.x < 0 || HeightMapTexcoord.y < 0 || HeightMapSize.x <= HeightMapTexcoord.x || HeightMapSize.y <= HeightMapTexcoord.y)
  {
    return;
  }

  HeightMap[HeightMapTexcoord] += smoothstep(0.0f, 1.0f, clamp(1.0f - (length(Offset) / BlushRadius), 0.0f, 1.0f)) * TerrainUpdateConstantData.BlushHardness;
}
