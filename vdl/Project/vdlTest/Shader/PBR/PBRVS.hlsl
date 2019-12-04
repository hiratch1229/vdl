#include "../Mesh/StaticMesh/StaticMesh.hlsli"
#include "PBR.hlsli"

PS_IN main(VS_IN In)
{
  PS_IN Out;

  float4 World = mul(In.Position, In.World);
  Out.Position = mul(World, ViewProjectionMatrix);
  Out.World = World.xyz;
  Out.Normal = normalize(mul(In.Normal, (float3x3) In.World));
  Out.Tangent = normalize(mul(In.Tangent, (float3x3) In.World));
  Out.Binormal = cross(Out.Normal, Out.Tangent);
  Out.Color = In.Color;
  Out.Texcoord = In.Texcoord;

  return Out;
}
