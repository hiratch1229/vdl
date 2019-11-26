float3 GetShadowColor(Texture2D _ShadowMap, SamplerState _Sampler, float4 _Position, row_major float4x4 _ViewProjection, float3 _Color, float _Bias)
{
  float4 ShadowPosition = mul(_Position, _ViewProjection);
  ShadowPosition /= ShadowPosition.w;
  ShadowPosition.xy = float2(ShadowPosition.x, -ShadowPosition.y) * 0.5f + 0.5f;

  float d = _ShadowMap.Sample(_Sampler, ShadowPosition.xy).r;
  return (ShadowPosition.z - d > _Bias ? _Color : 1.0f);
}
