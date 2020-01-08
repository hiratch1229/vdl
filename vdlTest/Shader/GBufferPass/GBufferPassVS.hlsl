#include "../Mesh/StaticMesh/StaticMesh.hlsli"
#include "GBufferPass.hlsli"

VS_OUT main(VS_IN In)
{
  VS_OUT Out;

  const float4 World = mul(In.Position, In.World);

  Out.Position = mul(World, ViewProjectionMatrix);
  Out.Texcoord = In.Texcoord;
  Out.Color = In.Color;
  Out.Normal = normalize(mul(In.Normal, (float3x3) In.World));
  Out.Tangent = normalize(mul(In.Tangent, (float3x3) In.World));
  Out.Binormal = cross(Out.Normal, Out.Tangent);

  return Out;
}
