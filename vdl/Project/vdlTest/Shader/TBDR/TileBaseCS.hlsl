#include "PointLight.hlsli"

Texture2D DiffuseGBuffer : register(t0);
Texture2D NormalGBuffer : register(t1);
Texture2D DepthBuffer : register(t2);
Texture2D ShadowMap : register(t3);
SamplerState ShadowMapSampler : register(s0);

RWStructuredBuffer<PointLight> PointLights : register(u1);
RWTexture2D<float4> OutputTexture : register(u2);

cbuffer Data : register(b1)
{
  float3 EyePosition;
  float SpecularPower;
  float3 Ambient;
  uint PointLightNum;
  float3 Shadow;
  float ShadowBias;
  row_major float4x4 InverseViewProjection;
};

cbuffer Light : register(b2)
{
  row_major float4x4 LightViewProjection;
};

cbuffer Camera : register(b3)
{
  row_major float4x4 View;
  row_major float4x4 Projection;
};

cbuffer LightData : register(b4)
{
  DirectionalLight DLight;
};

static const uint2 kTileSize = uint2(16, 16);
static const uint kTilePerThreadNum = kTileSize.x * kTileSize.y;
static const uint2 kWindowSize = uint2(1280, 720);

//  共有メモリ
groupshared uint gMinZ; //  タイルの最小深度
groupshared uint gMaxZ; //  タイルの最大深度
groupshared uint gTileLightIndices[kMaxPointLightNum]; //  タイルに接触しているポイントライトのインデックス
groupshared uint gTileNumLights; //  タイルに接触しているポイントライトの数
groupshared uint gNumPerSamplePixels;

[numthreads(kTileSize.x, kTileSize.y, 1)]
void main(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID)
{
  // タイル内でのインデックス
  uint GroupIndex = GroupThreadID.y * kTileSize.x + GroupThreadID.x;

  uint3 Texcoord = uint3(DispatchThreadID.xy, 0);

  float2 P = (Texcoord.xy + 0.5f) / kWindowSize;
  P = float2(P.x, 1.0f - P.y) * 2.0f - 1.0f;

  float ZBuffer = DepthBuffer.Load(Texcoord).r;

  float4 Position = mul(float4(P, ZBuffer, 1.0f), InverseViewProjection);
  Position /= Position.w;

  float4 Diffuse = DiffuseGBuffer.Load(Texcoord);
  float3 AmbientColor = Diffuse.rgb * Ambient.rgb;
  float3 Normal = NormalGBuffer.Load(Texcoord).rgb;
  Diffuse.rgb *= Calc(DLight, Normal);

  float3 H = normalize(-DLight.Direction + (EyePosition - Position.xyz));
       
  float4 ShadowPosition = mul(Position, LightViewProjection);
  ShadowPosition /= ShadowPosition.w;
  ShadowPosition.xy = float2(ShadowPosition.x, -ShadowPosition.y) * 0.5f + 0.5f;

  float d = ShadowMap[ShadowPosition.xy].r;
  float3 ShadowColor = (ShadowPosition.z - d > ShadowBias) ? Shadow : 1.0f;

  float4 PositionView = mul(Position, View);
  PositionView /= PositionView.w;

  //  共有メモリを初期化
  if (GroupIndex == 0)
  {
    gTileNumLights = 0;
    gNumPerSamplePixels = 0;
    gMinZ = 0x7F7FFFFF; // floatの最大値
    gMaxZ = 0;
  }
  
  //  同期
  GroupMemoryBarrierWithGroupSync();
  
  //  タイルの最大・最小深度を算出
  InterlockedMin(gMinZ, asuint(PositionView.z));
  InterlockedMax(gMaxZ, asuint(PositionView.z));
  
  //  同期
  GroupMemoryBarrierWithGroupSync();
  
  //  タイルの錐台を算出
  //  スレッド毎に行う必要は無いが、安価な為問題なし(Intel)
  float4 FrustumPlanes[6];
  {
    float MinTileZ = asfloat(gMinZ);
    float MaxTileZ = asfloat(gMaxZ);
  
    float2 TileScale = kWindowSize * rcp(float(2 * kTileSize.x));
    float2 TileBias = TileScale - float2(GroupID.xy);
  
    float4 c1 = float4(Projection[0].x * TileScale.x, 0.0f, TileBias.x, 0.0f);
    float4 c2 = float4(0.0f, -Projection[1].y * TileScale.y, TileBias.y, 0.0f);
    float4 c4 = float4(0.0f, 0.0f, 1.0f, 0.0f);
  
    FrustumPlanes[0] = c4 - c1;
    FrustumPlanes[1] = c4 + c1;
    FrustumPlanes[2] = c4 - c2;
    FrustumPlanes[3] = c4 + c2;
    FrustumPlanes[4] = float4(0.0f, 0.0f, 1.0f, -MinTileZ);
    FrustumPlanes[5] = float4(0.0f, 0.0f, -1.0f, MaxTileZ);
  
    //  側面の法線の正規化
    for (uint i = 0; i < 4; ++i)
    {
      FrustumPlanes[i] *= rcp(length(FrustumPlanes[i].xyz));
    }
  }
  
  //  タイルとポイントライトの当たり判定
  {
    for (uint LightIndex = GroupIndex; LightIndex < PointLightNum; LightIndex += kTilePerThreadNum)
    {
      PointLight Light = PointLights[LightIndex];
  
      bool isHit = true;
  
      float4 LightPos = mul(float4(Light.Position, 1.0f), View);
      LightPos /= LightPos.w;
      for (uint i = 0; i < 6; ++i)
      {
        float d = dot(FrustumPlanes[i], LightPos);
        isHit = isHit && (d >= -Light.Range);
      }
  
      if (isHit)
      {
        uint ListIndex;
        InterlockedAdd(gTileNumLights, 1, ListIndex);
        gTileLightIndices[ListIndex] = LightIndex;
      }
    }
  }
  
  //  同期
  GroupMemoryBarrierWithGroupSync();
  
  float3 LightColor = 0.0f;
  for (uint LightCount = 0; LightCount < gTileNumLights; ++LightCount)
  {
    LightColor += Calc(PointLights[gTileLightIndices[LightCount]], Position.xyz, Normal);
  }
  
  OutputTexture[Texcoord.xy] = float4((Diffuse.rgb + AmbientColor) * ShadowColor + LightColor, Diffuse.a);
}
