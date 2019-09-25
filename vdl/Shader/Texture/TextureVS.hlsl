#include "Texture.hlsli"

PS_IN main(VS_IN In)
{
  PS_IN Out;

  Out.Position = mul(In.Position, In.NDCTransform);
  Out.Color = In.Color;
  Out.Texcoord = In.Texcoord * In.TexcoordScale + In.TexcoordTranslate;

  return Out;
}
