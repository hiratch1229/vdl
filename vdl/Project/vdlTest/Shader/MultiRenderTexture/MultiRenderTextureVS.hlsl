#include "MultiRenderTexture.hlsli"

PS_IN main(VS_IN In)
{
  PS_IN Out;

  const float4 World = mul(In.Position, In.World);

  Out.Position = mul(World, ViewProjectionMatrix);
  Out.Color = In.Color;
  Out.Texcoord = In.Texcoord;
  
  return Out;
}
