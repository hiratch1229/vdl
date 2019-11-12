#include "../Mesh/StaticMesh/StaticMesh.hlsli"
#include "Skybox.hlsli"

PS_IN main(VS_IN In)
{
  PS_IN Out;

  Out.Position = mul(mul(In.Position, In.World), ViewProjectionMatrix).xyzw;
  Out.Texcoord = In.Position.xyz;

  return Out;
}
