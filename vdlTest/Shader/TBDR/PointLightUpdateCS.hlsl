#include "PointLight.hlsli"

RWStructuredBuffer<Data> Datas : register(u0);
RWStructuredBuffer<PointLight> PointLights : register(u1);

cbuffer ConstantBuffer : register(b0)
{
  float DeltaTime;
  float PointLightItensity;
  float PointLightRange;
  float Padding;
};

static const uint3 kThreadNum = uint3(1024, 1, 1);

float QuadraticFunction(float _Time, float _TotalTime, float _Min, float _Max)
{
  return (1.0f - (_Time / (_TotalTime / 2.0f) - 1.0f) * (_Time / (_TotalTime / 2.0f) - 1.0f)) * (_Max - _Min) + _Min;
}

[numthreads(kThreadNum.x, kThreadNum.y, kThreadNum.z)]
void main(uint3 DID : SV_DispatchThreadID)
{
  const uint Index = DID.x;

  if (Index >= kMaxPointLightNum)
  {
    return;
  }

  //  データの更新
  {
    Datas[Index].Timer += DeltaTime;
    while (Datas[Index].Timer > Datas[Index].Time)
    {
      Datas[Index].Timer -= Datas[Index].Time;
    }
  }
  
  //  ポイントライトの更新
  {
    PointLights[Index].Position = float3(QuadraticFunction(Datas[Index].Timer, Datas[Index].Time, Datas[Index].MinRange.x, Datas[Index].MaxRange.x),
        QuadraticFunction(Datas[Index].Timer, Datas[Index].Time, Datas[Index].MinRange.y, Datas[Index].MaxRange.y), QuadraticFunction(Datas[Index].Timer, Datas[Index].Time, Datas[Index].MinRange.z, Datas[Index].MaxRange.z));
    PointLights[Index].Color = Datas[Index].Color;
    PointLights[Index].Itensity = PointLightItensity;
    PointLights[Index].Range = PointLightRange;
  }
}
