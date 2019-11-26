#include "StaticMesh.hlsli"
#include "../Deferred.hlsli"

PS_OUT_GBUFFER main(PS_IN In)
{
  PS_OUT_GBUFFER Out;
  Out.Diffuse = In.Color;
  Out.Normal = float4(In.Normal, 1.0f);
  Out.Depth = float4(In.Position.z, 0.0f, 0.0f, 1.0f);

  return Out;
}
