static const float kKernelOffsets[3] = { 0.0f, 1.3846153846f, 3.2307692308f };
static const float kBlurWeights[3] = { 0.2270270270f, 0.3162162162f, 0.0702702703f };

float3 VerticalGaussianBlur(Texture2D _Texture, SamplerState _Sampler, float2 _Texcoord)
{
  uint2 Size;
  _Texture.GetDimensions(Size.x, Size.y);

  float3 Color = _Texture.Sample(_Sampler, _Texcoord).rgb * kBlurWeights[0];

  for (uint i = 1; i < 3; ++i)
  {
    float2 NormalizeOffset = float2(0.0f, kKernelOffsets[i] / Size.y);
    Color += _Texture.Sample(_Sampler, _Texcoord + NormalizeOffset).rgb * kBlurWeights[i];
    Color += _Texture.Sample(_Sampler, _Texcoord - NormalizeOffset).rgb * kBlurWeights[i];
  }

  return Color;
}

float3 HorizontalGaussianBlur(Texture2D _Texture, SamplerState _Sampler, float2 _Texcoord)
{
  uint2 Size;
  _Texture.GetDimensions(Size.x, Size.y);

  float3 Color = _Texture.Sample(_Sampler, _Texcoord).rgb * kBlurWeights[0];

  for (uint i = 1; i < 3; ++i)
  {
    float2 NormalizeOffset = float2(kKernelOffsets[i] / Size.x, 0.0f);
    Color += _Texture.Sample(_Sampler, _Texcoord + NormalizeOffset).rgb * kBlurWeights[i];
    Color += _Texture.Sample(_Sampler, _Texcoord - NormalizeOffset).rgb * kBlurWeights[i];
  }

  return Color;
}
