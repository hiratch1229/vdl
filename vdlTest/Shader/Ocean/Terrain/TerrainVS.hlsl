#include "../../Mesh/StaticMesh/StaticMesh.hlsli"
#include "Terrain.hlsli"
#include "../Tessellation.hlsli"

HS_IN main(VS_IN In, uint ID : SV_InstanceID)
{
  HS_IN Out;

  Out.Position = mul(In.Position, In.World);
  Out.Normal = normalize(mul(In.Normal, (float3x3) In.World));
  Out.Tangent = normalize(mul(In.Tangent, (float3x3) In.World));
  Out.Color = In.Color;
  Out.Texcoord = CalcTexcoord(In.Texcoord, ID);

  return Out;
}
