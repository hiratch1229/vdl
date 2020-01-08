RWTexture2D<float> HeightMap : register(u0);

cbuffer ConstantBuffer : register(b0)
{
  float3 RayPosition;
  float BlushScale;
  float3 RayDirection;
  float BlushHardness;
};

static const uint2 kHeightMapSize = uint2(1024, 1024);
static const float2 kHeightMapHalfSize = kHeightMapSize / 2.0f;
static const uint2 kThreadNum = uint2(32, 32);
static const uint2 kCalcVertexNum = kHeightMapSize / kThreadNum;
static const float2 kRectangleSize = uint2(10, 10) * float2(25.0f, 25.0f);
static const float2 kVertexIntarval = kRectangleSize / kHeightMapSize;

struct Triangle
{
  float3 Positions[3];
};

bool isCollision(Triangle _Triangle, float3 _RayPosition, float3 _RayDirection, out float3 _Intersection)
{
  const float3 l0 = _Triangle.Positions[1] - _Triangle.Positions[0];
  const float3 l1 = _Triangle.Positions[2] - _Triangle.Positions[1];
  const float3 l2 = _Triangle.Positions[0] - _Triangle.Positions[2];

  const float3 Normal = cross(l0, l1);
  const float Dot = dot(_RayDirection, Normal);
  _Intersection = _RayDirection * (dot(Normal, _Triangle.Positions[0] - _RayPosition) / Dot) + _RayPosition;

  return dot(_RayDirection, Normal) <= 0.0f && dot(cross(_Triangle.Positions[0] - _Intersection, l0), Normal) >= 0.0f
    && dot(cross(_Triangle.Positions[1] - _Intersection, l1), Normal) >= 0.0f && dot(cross(_Triangle.Positions[2] - _Intersection, l2), Normal) >= 0.0f;
}

float3 CalcVertexPosition(uint2 _Texcoord)
{
  return float3((_Texcoord.x - kHeightMapHalfSize.x) * kVertexIntarval.x, HeightMap[_Texcoord].r, (kHeightMapSize.y - (_Texcoord.y - kHeightMapHalfSize.y)) * kVertexIntarval.y);
}

[numthreads(kThreadNum.x, kThreadNum.y, 1)]
void main(uint3 DID : SV_DispatchThreadID)
{
  uint2 LeftTopTexcoord = uint2(DID.x * kCalcVertexNum.x, DID.y * kCalcVertexNum.y);
  float BlushHalfScale = BlushScale * 0.5f;

  for (uint y = 1; y < kCalcVertexNum.y; ++y)
  {
    for (uint x = 1; x < kCalcVertexNum.x; ++x)
    {
      Triangle Triangles[2];
      {
        Triangles[0].Positions[0] = CalcVertexPosition(LeftTopTexcoord + uint2(-1, -1)); //  LeftTop
        Triangles[0].Positions[1] = Triangles[1].Positions[1] = CalcVertexPosition(LeftTopTexcoord + uint2(+0, -1)); //  RightTop
        Triangles[0].Positions[2] = Triangles[1].Positions[0] = CalcVertexPosition(LeftTopTexcoord + uint2(-1, +0)); //  LeftBottom
        Triangles[1].Positions[2] = CalcVertexPosition(LeftTopTexcoord + uint2(+0, +0)); //  RightBottom
      }

      for (uint i = 0; i < 2; ++i)
      {
        float3 Intersection;
        if (isCollision(Triangles[i], RayPosition, RayDirection, Intersection))
        {
          float2 Texcoord = ((float2(Intersection.x, Intersection.z) / kRectangleSize) * float2(1.0f, -1.0f) + 0.5f) * kHeightMapSize;

          for (float OffsetX = -BlushHalfScale; OffsetX < BlushHalfScale; ++OffsetX)
          {
            for (float OffsetY = -BlushHalfScale; OffsetY < BlushHalfScale; ++OffsetY)
            {
              float2 Offset = float2(OffsetX, OffsetY);

              HeightMap[Texcoord + Offset].r += BlushHardness * smoothstep(0.0f, 1.0f, length(BlushHalfScale) - length(Offset));
            }
          }
        }
      }
    }
  }
}
