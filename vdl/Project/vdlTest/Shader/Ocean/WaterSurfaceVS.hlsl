#include "WaterSurface.hlsli"
#include "../Mesh/StaticMesh/StaticMesh.hlsli"

HS_IN main(VS_IN In)
{
  HS_IN Out;

  Out.Position = mul(In.Position, In.World);
  Out.Normal = normalize(mul(In.Normal, (float3x3) In.World));
  Out.Color = In.Color;
  Out.Texcoord = In.Texcoord;
  
  return Out;
}
