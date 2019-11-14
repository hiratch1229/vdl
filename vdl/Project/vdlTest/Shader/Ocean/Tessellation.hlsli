struct ConstantData
{
  float TessFactor[4] : SV_TessFactor;
  float InsideTessFactor[2] : SV_InsideTessFactor;
};

#define DOMAIN "quad"
#define CONTROL_POINT 4

static const uint2 kRectangleNum = uint2(10, 10);

float2 CalcTexcoord(float2 _Texcoord, uint _InstanceID)
{
  return (float2(_InstanceID % kRectangleNum.x, kRectangleNum.y - (_InstanceID / kRectangleNum.x) - 1) + _Texcoord) / kRectangleNum;
}
