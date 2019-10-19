#include "LightPass.hlsli"
#include "../Option/Light.hlsli"

Texture2D DiffuseGBuffer : register(t0);
Texture2D NormalGBuffer : register(t1);
Texture2D DepthBuffer : register(t2);

static const uint kPointLightNum = 200;
static const uint2 kWindowSize = uint2(1280, 720);

cbuffer LightData : register(b0)
{
  DirectionalLight DLight;
  PointLight PLights[kPointLightNum];
};

cbuffer Data : register(b1)
{
  //float4 EyePosition;
  float4x4 InverseViewProjection;
};

float4 main(VS_OUT In) : SV_TARGET
{
  int3 TexCoord = int3(In.Position.xy, 0);

  float2 P = In.Position.xy / kWindowSize;
  P = float2(P.x, 1.0f - P.y) * 2.0f - 1.0f;

  float4 Position = mul(InverseViewProjection, float4(P, DepthBuffer.Load(TexCoord).r, 1.0f));
  Position /= Position.w;

  float4 Diffuse = DiffuseGBuffer.Load(TexCoord);
  float4 Normal = NormalGBuffer.Load(TexCoord);

  float4 Color = 0.0f;
 
  Color += Calc(DLight, Normal.xyz);
  for (int i = 0; i < kPointLightNum; ++i)
  {
    Color += Calc(PLights[i], Position.xyz, Normal.xyz);
  }
  
  Color.rgb += Diffuse.rgb;
  Color.a = Diffuse.a;

  return Color;
}
