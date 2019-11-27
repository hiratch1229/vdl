static const uint kMaxRayMarchSampleNum = 100;
static const float kFadeDistance = 0.5f;
static const float kFadeEdge = 2.5f;
static const uint kBinarySearchSampleNum = 4;
//static const float kEdge = 0.05f;

float3 GetScreenSpaceReflectionVector(float3 _ScreenSpacePosition, float3 _Position, float3 _ReflectVector, row_major float4x4 _ViewProjection)
{
  float4 PointAlongReflectVector = float4(_Position + 10.0f * _ReflectVector, 1.0f);
  
  float4 ScreenSpaceReflectionPoint = mul(PointAlongReflectVector, _ViewProjection);
  ScreenSpaceReflectionPoint /= ScreenSpaceReflectionPoint.w;
  ScreenSpaceReflectionPoint.xy = float2(ScreenSpaceReflectionPoint.x, -ScreenSpaceReflectionPoint.y) * 0.5f + 0.5f;

  return normalize(ScreenSpaceReflectionPoint.xyz - _ScreenSpacePosition);
}

void BinarySearch(Texture2D _DepthBuffer, SamplerState _Sampler, float3 _ScreenSpaceVector, inout float3 _RayPosition, inout float _ZBuffer)
{
  float3 MinRayPosition = _RayPosition -= _ScreenSpaceVector;
  float3 MaxRayPosition = _RayPosition;
      
  for (uint j = 0; j < kBinarySearchSampleNum; ++j)
  {
    _RayPosition = lerp(MinRayPosition, MaxRayPosition, 0.5f);
    _ZBuffer = _DepthBuffer.Sample(_Sampler, _RayPosition.xy).x;
        
    if (_RayPosition.z > _ZBuffer)
    {
      MaxRayPosition = _RayPosition;
    }
    else
    {
      MinRayPosition = _RayPosition;
    }
  }
      
  _RayPosition = lerp(MinRayPosition, MaxRayPosition, 0.5f);
  _ZBuffer = _DepthBuffer.Sample(_Sampler, _RayPosition.xy).x;
}

//  http://casual-effects.blogspot.com/2014/08/screen-space-ray-tracing.html
bool ScreenSpaceRayMarch(Texture2D _DepthBuffer, SamplerState _Sampler, float3 _ScreenSpacePosition, float3 _ScreenSpaceReflectionVector, uint _SampleNum, out float3 _RayPosition, out float _ZBuffer)
{
  float3 RayPosition;
  float ZBuffer;
  
  _SampleNum = min(_SampleNum, kMaxRayMarchSampleNum);
  [unroll(kMaxRayMarchSampleNum)]
  for (uint i = 1; i < _SampleNum; ++i)
  {
    RayPosition = _ScreenSpacePosition + (i * _ScreenSpaceReflectionVector);

    ////  ‰æ–ÊŠO
    //if (RayPosition.x < kEdge || RayPosition.y < kEdge || RayPosition.x > (1.0f - kEdge) || RayPosition.y > (1.0f - kEdge) || RayPosition.z >= 1.0f)
    //{
    //  continue;
    //}

    ZBuffer = _DepthBuffer.Sample(_Sampler, RayPosition.xy).x;

    if (ZBuffer < _ScreenSpacePosition.z || ZBuffer <= RayPosition.z)
    {
      //BinarySearch(_DepthBuffer, _Sampler, _ScreenSpaceReflectionVector, RayPosition, ZBuffer);
      
      _RayPosition = RayPosition;
      _ZBuffer = ZBuffer;
      
      return true;
    }
  }
  
  return false;
}

float DistanceFead(float2 _ScreenSpacePosition, float2 _RayPosition)
{
  float DistanceFactor = length(_ScreenSpacePosition - _RayPosition) / kFadeDistance;
  return saturate(1.0f - (DistanceFactor * DistanceFactor));
}

float EdgeFead(float2 _RayPosition)
{
  float EdgeFactor = length(_RayPosition - float2(0.5f, 0.5f)) * kFadeEdge;
  return saturate(1.0f - (EdgeFactor * EdgeFactor));
}
