#include "LightPass.hlsli"
#include "../Option/Light.hlsli"

Texture2D DiffuseGBuffer : register(t0);
Texture2D NormalGBuffer : register(t1);
Texture2D SpecularGBuffer : register(t2);
Texture2D DepthBuffer : register(t3);

static const uint kPointLightNum = 200;
static const uint2 kWindowSize = uint2(1280, 720);

cbuffer LightData : register(b0)
{
  DirectionalLight DLight;
  PointLight PLights[kPointLightNum];
};

cbuffer Data : register(b1)
{
  float3 EyePosition;
  float SpecularPower;
  float4 Ambient;
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
  float3 Normal = NormalGBuffer.Load(TexCoord).rgb;

  float3 LightColor = 0.0f;
  {
    LightColor += Calc(DLight, Normal);
    for (int i = 0; i < kPointLightNum; ++i)
    {
      LightColor += Calc(PLights[i], Position.xyz, Normal);
    }
  }

  float3 H = normalize(-DLight.Direction + (EyePosition - Position.xyz));

  float3 SpecularColor = SpecularGBuffer.Load(TexCoord).rgb;

  float3 AmbientColor = Diffuse.rgb * Ambient.rgb;

  return float4(Diffuse.rgb + LightColor + AmbientColor + SpecularColor * Specular(H, Normal, SpecularPower), Diffuse.a);
}
