float4 main(uint ID : SV_VertexID)
{
  return float4((float2(ID % 2, 1.0f - ID / 2) * 2.0f) - 1.0f, 0.0f, 1.0f);;
}
