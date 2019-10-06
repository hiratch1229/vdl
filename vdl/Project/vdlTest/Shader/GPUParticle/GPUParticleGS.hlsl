#include "GPUParticle.hlsli"

cbuffer ConstantBuffer : register(b0)
{
  float2 PointHalfSize;
};

[maxvertexcount(4)]
void main(point GS_IN In[1], inout TriangleStream<PS_IN> OutStream)
{
  PS_IN Out;

  Out.Color = In[0].Color;

  //  ����
  {
    Out.Position = float4(((In[0].Position.xy + float2(-PointHalfSize.x, -PointHalfSize.y)) / kWindowSize) * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), In[0].Position.zw);
    Out.Texcoord = float2(0.0f, 0.0f);

    OutStream.Append(Out);
  }

  //  �E��
  {
    Out.Position = float4(((In[0].Position.xy + float2(+PointHalfSize.x, -PointHalfSize.y)) / kWindowSize) * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), In[0].Position.zw);
    Out.Texcoord = float2(1.0f, 0.0f);

    OutStream.Append(Out);
  }

  //  ����
  {
    Out.Position = float4(((In[0].Position.xy + float2(-PointHalfSize.x, +PointHalfSize.y)) / kWindowSize) * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), In[0].Position.zw);
    Out.Texcoord = float2(0.0f, 1.0f);

    OutStream.Append(Out);
  }

  //  �E��
  {
    Out.Position = float4(((In[0].Position.xy + float2(+PointHalfSize.x, +PointHalfSize.y)) / kWindowSize) * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), In[0].Position.zw);
    Out.Texcoord = float2(1.0f, 1.0f);

    OutStream.Append(Out);
  }

  OutStream.RestartStrip();
}
