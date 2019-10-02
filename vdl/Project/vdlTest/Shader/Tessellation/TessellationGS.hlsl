#include "Tessellation.hlsli"

[domain("tri")]
DS_OUT main(TriangleConstantData Input, float3 Domain : SV_DomainLocation, const OutputPatch<HS_OUT, CONTROL_POINT_TRIANGLE> OPatch)
{
  DS_OUT Out;

  Out.Position = OPatch[0].Position * Domain.x + OPatch[1].Position * Domain.y + OPatch[2].Position * Domain.z;
  Out.Position.w = 1.0f;
  Out.Color = OPatch[0].Color * Domain.x + OPatch[1].Color * Domain.y + OPatch[2].Color * Domain.z;

  return Out;
}
