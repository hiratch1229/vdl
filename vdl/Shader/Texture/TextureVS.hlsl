#include "Texture.hlsli"

VS_OUT main(VS_IN In)
{
  VS_OUT Out;

  Out.Position = mul(In.Position, In.NDCTransform);
  Out.Color = In.Color;
  Out.Texcoord = In.Texcoord * In.TexcoordScale + In.TexcoordTranslate;

  return Out;
}
