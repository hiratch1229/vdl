#include "../../Option/Light.hlsli"

struct CameraData
{
  row_major float4x4 ViewProjection;
  row_major float4x4 InverseViewProjection;
  float3 EyePosition;
  float FocalLength;
  float3 ViewVector;
  uint Unused;
};

struct LightData
{
  row_major float4x4 ViewProjection;
  DirectionalLight DLight;
};

struct TerrainUpdateData
{
  uint2 MousePosition;
  float BlushSize;
  float BlushHardness;
};

struct TerrainData
{
  uint TextureLoopNum;
  float SandThreshold;
  float RockThreshold;
  float SlopeThreshold;
};

struct Wave
{
  float3 Direction;
  float Length;
  float Speed;
  float Steepness;
  float Amplitude;
  float Unused;
};

static const uint kMaxWaveNum = 25;
struct WaveData
{
  Wave Waves[kMaxWaveNum];
};

struct WaterSurfaceData
{
  float Time;
  uint WaveNum;
};

struct ShadowData
{
  float3 Color;
  float Bias;
};

struct RayMarchData
{
  float MaxStep;
  uint SampleNum;
  float Step;
  float Unused;
};

struct DepthOfFieldData
{
  row_major float4x4 OrthographicViewProjection;
};
