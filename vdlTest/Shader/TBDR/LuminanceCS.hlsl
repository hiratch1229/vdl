Texture2D LightTexture : register(t3);
RWTexture2D<float4> LuminanceMap : register(u3);

cbuffer BloomData : register(b4)
{
  float LuminanceThreshold;
  float Exposure;
};

static const uint2 kThreadGroupNum = uint2(32, 32);
static const uint2 kLuminanceTextureSize = uint2(1280, 720);

float GetLuminance(float3 _Color)
{
  return _Color.r * 0.29891f + _Color.g * 0.58661f + _Color.b * 0.11448f;
}

[numthreads(kThreadGroupNum.x, kThreadGroupNum.y, 1)]
void main(uint3 DispatchThreadID : SV_DispatchThreadID)
{
  const uint2 Texcoord = DispatchThreadID.xy;

  const float3 Color = LightTexture[Texcoord].rgb;
  
  LuminanceMap[Texcoord] = float4(Color * max(0.0f, GetLuminance(Color) - LuminanceThreshold) * Exposure, 1.0f);
}
