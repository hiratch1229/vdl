struct VS_IN
{
  float4 Position : POSITION;
  float3 Normal : NORMAL;
  float3 Tangent : TANGENT;
  float2 Texcoord : TEXCOORD;
  float4 BoneWeights : WEIGHTS;
  uint4 BoneIndices : BONES;

  row_major float4x4 World : WORLD;
  float4 Color : COLOR;
};

cbuffer ConstantBuffer : register(b0)
{
  row_major float4x4 ViewProjectionMatrix;
};
