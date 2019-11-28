#include "../../Texture/Texture.hlsli"
#include "../PostProcess.hlsli"
#include "../ConstantBuffers.hlsli"

SamplerState Sampler : register(s0);
Texture2D Texture : register(t0);
Texture2D BlurTexture : register(t1);
Texture2D DepthTexture : register(t2);
cbuffer ConstantBuffer : register(b0)
{
  CameraData CameraConstantData;
}
cbuffer ConstantBuffer : register(b1)
{
  DepthOfFieldData DepthOfFieldConstantData;
}

float ConversionDepth(float2 _Position, float _Depth)
{
  float4 Position = GetWorldPosition(float3(GetNDCPosition(_Position.xy), _Depth), CameraConstantData.InverseViewProjection);
  float4 ScreenSpacePosition = mul(Position, DepthOfFieldConstantData.OrthographicViewProjection);
  
  return ScreenSpacePosition.z / ScreenSpacePosition.w;
}

float4 main(PS_IN In) : SV_TARGET
{
  //float FocalLength = ConversionDepth(In.Position.xy, DepthTexture.Sample(Sampler, float2(0.5f, 0.5f)).r);
  float Depth = ConversionDepth(In.Position.xy, DepthTexture.Sample(Sampler, In.Texcoord).r);
  
  float4 Color = Texture.Sample(Sampler, In.Texcoord);
  float4 BlurColor = BlurTexture.Sample(Sampler, In.Texcoord);
  
  //float Length = Depth - FocalLength;
  float Length = Depth - CameraConstantData.FocalLength;
  return lerp(Color, BlurColor, abs(Length));
}
