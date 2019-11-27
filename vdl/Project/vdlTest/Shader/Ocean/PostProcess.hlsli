struct WaterSurfacePSOut
{
  float4 Color : SV_TARGET0;
  float4 Luminance : SV_TARGET1;
};

static const float2 kWindowSize = float2(1280, 720);

float2 GetNDCPosition(float2 _Position)
{
  return _Position.xy / kWindowSize;
}

float4 GetWorldPosition(float3 _ScreenSpacePosition, row_major float4x4 _InverseViewProjection)
{
  float3 NDCPosition = float3(float2(_ScreenSpacePosition.x, 1.0f - _ScreenSpacePosition.y) * 2.0f - 1.0f, _ScreenSpacePosition.z);

  float4 Position = mul(float4(NDCPosition, 1.0f), _InverseViewProjection);
  Position /= Position.w;

  return Position;
}

float GetLuminance(float3 _Color)
{
  return _Color.r * 0.29891f + _Color.g * 0.58661f + _Color.b * 0.11448f;
}
