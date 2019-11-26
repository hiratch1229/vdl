#include "../ConstantBuffers.hlsli"

Texture2D TexcoordMap : register(t0);
RWTexture2D<float> HeightMap : register(u0);
cbuffer ConstantBuffer : register(b0)
{
  TerrainUpdateData TerrainUpdateConstantData;
}

static const uint2 kWindowSize = uint2(1280, 720);
static const uint2 kTexcoordMapSize = uint2(1280, 720);
static const uint2 kHeightMapSize = uint2(1024, 1024);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
  const float2 Texcoord = TexcoordMap[(TerrainUpdateConstantData.MousePosition / (float2) kWindowSize) * kTexcoordMapSize].xy;
  if (Texcoord.x < 0.0f || Texcoord.y < 0.0f)
  {
    return;
  }

  const float BlushRadius = TerrainUpdateConstantData.BlushSize * 0.5f;

  for (float y = -BlushRadius; y < BlushRadius; ++y)
  {
    for (float x = -BlushRadius; x < BlushRadius; ++x)
    {
      const float2 Offset = float2(x, y);

      const int2 HeightMapTexcoord = (Texcoord + (Offset / kHeightMapSize)) * kHeightMapSize;

      if (HeightMapTexcoord.x <= 0 || HeightMapTexcoord.y <= 0 || (kHeightMapSize.x - 1) <= HeightMapTexcoord.x || (kHeightMapSize.y - 1) <= HeightMapTexcoord.y)
      {
        continue;
      }

      HeightMap[HeightMapTexcoord] += smoothstep(0.0f, 1.0f, clamp(1.0f - (length(Offset) / BlushRadius), 0.0f, 1.0f)) * TerrainUpdateConstantData.BlushHardness;
    }
  }
}
