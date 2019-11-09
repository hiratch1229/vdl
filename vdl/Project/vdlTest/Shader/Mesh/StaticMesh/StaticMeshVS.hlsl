#include "StaticMesh.hlsli"
#include "../Mesh.hlsli"

PS_IN main(VS_IN In)
{
  PS_IN Out;

  const float4 World = mul(In.Position, In.World);

  Out.Position = mul(World, ViewProjectionMatrix);
  Out.Texcoord = In.Texcoord;
  Out.Color = In.Color;
  
  return Out;
}
