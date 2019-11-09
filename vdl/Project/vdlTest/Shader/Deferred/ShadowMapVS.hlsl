#include "../Mesh/StaticMesh/StaticMesh.hlsli"

cbuffer Light : register(b1)
{
  row_major float4x4 LightViewProjection;
};

float4 main(VS_IN In) : SV_POSITION
{
  return mul(mul(In.Position, In.World), LightViewProjection);
}
