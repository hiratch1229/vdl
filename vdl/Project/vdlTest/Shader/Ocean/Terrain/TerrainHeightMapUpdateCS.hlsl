Texture2D TexcoordMap : register(t0);
RWTexture2D<float> HeightMap : register(u0);

cbuffer ConstantBuffer : register(b0)
{
  uint2 MousePosition;
  float BlushSize;
  float BlushHardness;
};

static const uint2 kWindowSize = uint2(1280, 720);
static const uint2 kTexcoordMapSize = uint2(1280, 720);
static const uint2 kHeightMapSize = uint2(1024, 1024);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
  const float2 Texcoord = TexcoordMap[(MousePosition / (float2) kWindowSize) * kTexcoordMapSize].xy;
  //const float BlushRadius = (BlushSize * 0.5f) / kHeightMapSize;
  const float BlushRadius = BlushSize * 0.5f;

  for (float y = -BlushRadius; y < BlushRadius; ++y)
  {
    for (float x = -BlushRadius; x < BlushRadius; ++x)
    {
      float2 Offset = float2(x, y) / kHeightMapSize;

      HeightMap[(Texcoord + Offset) * kHeightMapSize].r += BlushHardness * smoothstep(0.0f, 1.0f, clamp(BlushSize - length(Offset), 0.0f, 1.0f));
    }
  }
}
