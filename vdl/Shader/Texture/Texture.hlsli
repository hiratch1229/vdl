struct VS_IN
{
  float4 Position : POSITION;
  float2 Texcoord : TEXCOORD;

  column_major float4x4 NDCTransform : NDC_TRANSFORM;
  float2 TexcoordScale : TEXCOORD_SCALE;
  float2 TexcoordTranslate : TEXCOORD_TRANSLATE;
  float4 Color : COLOR;
};

struct VS_OUT
{
  float4 Position : SV_POSITION;
  float4 Color : COLOR;
  float2 Texcoord : TEXCOORD;
};
