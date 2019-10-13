#include "../Mesh/Mesh.hlsli"
#include "Tessellation.hlsli"

static const float4 kColors[4] =
{
  float4(1.0f, 0.0f, 0.0f, 1.0f),
  float4(0.0f, 1.0f, 0.0f, 1.0f),
  float4(0.0f, 0.0f, 1.0f, 1.0f),
  float4(1.0f, 1.0f, 1.0f, 1.0f)
};

VS_OUT main(VS_IN In, uint ID : SV_VertexID)
{
  VS_OUT Out;
  Out.Position = mul(mul(In.Position, In.World), ViewProjectionMatrix);
  Out.Color = kColors[ID];

  return Out;
}
