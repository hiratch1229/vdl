#include "GBufferPass.hlsli"

#define MAX_BONES 32
cbuffer CONSTANT_BUFFER : register(b0)
{
  row_major float4x4 ViewProjectionMatrix;
  row_major float4x4 BoneTransforms[MAX_BONES];
};

VS_OUT main(VS_IN In)
{
  VS_OUT Out;

  const float4 World = mul(In.Position, In.World);

  Out.Position = mul(World, ViewProjectionMatrix);
  Out.Texcoord = In.Texcoord;
  Out.Color = In.Color;
  Out.Normal = normalize(mul(In.Normal.xyz, (float3x3) In.World));
  Out.Tangent = normalize(mul(In.Tangent, (float3x3) In.World));
  Out.Binormal = cross(Out.Normal, Out.Tangent);

  return Out;
}
