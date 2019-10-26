#include "../Mesh/Mesh.hlsli"
#include "GBufferPass.hlsli"

VS_OUT main(VS_IN In)
{
  VS_OUT Out;

  const float4 World = mul(In.Position, In.World);

  Out.Position = mul(World, ViewProjectionMatrix);
  Out.Texcoord = In.Texcoord;
  Out.Normal = normalize(mul(In.Normal.xyz, (float3x3) In.World));
  Out.Color = In.Color;
  Out.Tangent = normalize(mul(In.Tangent, (float3x3) In.World));
  Out.Binormal = cross(Out.Normal, Out.Tangent);

  return Out;
}
