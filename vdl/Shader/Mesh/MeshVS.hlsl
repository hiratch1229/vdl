#include "Mesh.hlsli"

PS_IN main(VS_IN In)
{
  //float3 Position = float3(0.0f, 0.0f, 0.0f);
  //float3 Normal = float3(0.0f, 0.0f, 0.0f);
  //
  //for (int i = 0; i < 4; ++i)
  //{
  //  Position += (In.BoneWeights[i] * mul(In.Position, BoneTransforms[In.BoneIndices[i]])).xyz;
  //  Normal += (In.BoneWeights[i] * mul(float4(In.Normal.xyz, 0.0f), BoneTransforms[In.BoneIndices[i]])).xyz;
  //}
  //
  //In.Position = float4(Position, 1.0f);
  //In.Normal = float4(Normal, 0.0f);

  PS_IN Out;

  const float4 World = mul(In.Position, In.World);

  Out.Position = mul(World, ViewProjectionMatrix);
  Out.Texcoord = In.Texcoord;
  Out.Color = In.Color;
  
  return Out;
}
