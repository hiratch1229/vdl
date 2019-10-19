#include "LightPass.hlsli"

VS_OUT main(uint ID : SV_VertexID)
{
  VS_OUT Out;
  Out.Position = float4((float2(ID % 2, 1.0f - ID / 2) * 2.0f) - 1.0f, 0.0f, 1.0f);

  return Out;
}