#include "Tessellation.hlsli"

[domain("quad")]
DS_OUT main(QuadConstantData Input, float2 UV : SV_DomainLocation, const OutputPatch<HS_OUT, CONTROL_POINT_QUAD> OPatch)
{
  DS_OUT Out;

  Out.Position = lerp(lerp(OPatch[1].Position, OPatch[0].Position, UV.x), lerp(OPatch[3].Position, OPatch[2].Position, UV.x), UV.y);
  Out.Color = lerp(lerp(OPatch[1].Color, OPatch[0].Color, UV.x), lerp(OPatch[3].Color, OPatch[2].Color, UV.x), UV.y);

  return Out;
}
