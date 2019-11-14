#include "../../Mesh/StaticMesh/StaticMesh.hlsli"
#include "WaterSurface.hlsli"
#include "../Tessellation.hlsli"

HS_IN main(VS_IN In, uint ID : SV_InstanceID)
{
  HS_IN Out;

  Out.Position = mul(In.Position, In.World);
  Out.Color = In.Color;
  Out.Texcoord = CalcTexcoord(In.Texcoord, ID);

  return Out;
}
