#include "Deferred.hlsli"

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
  Out.Normal = mul(World, In.Normal.xyz);
  Out.Tangent = mul(World, In.Tangent);
  Out.Binormal = cross(In.Tangent, In.Normal.xyz);

  return Out;
}
