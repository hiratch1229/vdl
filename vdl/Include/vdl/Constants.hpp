#pragma once
#include "Types.hpp"
#include "Type2.hpp"

namespace vdl::Constants
{
  constexpr Type2<uint> kDefaultWindowSize = { 1280, 720 };

  constexpr uint kDefaultMaxFPS = 60;

  constexpr uint kMaxBoneNum = 32;

  constexpr uint kMaxBoneInfluence = 4;

  constexpr uint kMaxController = 4;

  constexpr uint kMaxCharacterNum = 256;

  constexpr uint kMaxRenderTextureNum = 8;

  constexpr uint kMaxSamplerNum = 8;

  constexpr uint kMaxShaderResourceNum = 32;

  constexpr uint kMaxConstantBufferNum = 14;

  constexpr uint kMaxUnorderedAccessObjectNum = 8;

  constexpr const char kDefaultTextureShaderCode[] = {
    "struct VS_IN"
    "{"
    "  float4 Position : POSITION;"
    "  float2 Texcoord : TEXCOORD;"
    ""
    "  column_major float4x4 NDCTransform : NDC_TRANSFORM;"
    "  float2 TexcoordScale : TEXCOORD_SCALE;"
    "  float2 TexcoordTranslate : TEXCOORD_TRANSLATE;"
    "  float4 Color : COLOR;"
    "};"
    ""
    "struct PS_IN"
    "{"
    "  float4 Position : SV_POSITION;"
    "  float4 Color : COLOR;"
    "  float2 Texcoord : TEXCOORD;"
    "};"
    ""
    "PS_IN VSMain(VS_IN In)"
    "{"
    "  PS_IN Out;"
    ""
    "  Out.Position = mul(In.Position, In.NDCTransform);"
    "  Out.Color = In.Color;"
    "  Out.Texcoord = In.Texcoord * In.TexcoordScale + In.TexcoordTranslate;"
    ""
    "  return Out;"
    "}"
    ""
    "Texture2D Texture : register(t0);"
    "SamplerState Sampler : register(s0);"
    ""
    "float4 PSMain(PS_IN In) : SV_TARGET"
    "{"
    "  return Texture.Sample(Sampler, In.Texcoord) * In.Color;"
    "}"
  };

  constexpr const char kDefaultStaticMeshShaderCode[] = {
    "struct VS_IN"
    "{"
    "  float4 Position : POSITION;"
    "  float3 Normal : NORMAL;"
    "  float3 Tangent : TANGENT;"
    "  float2 Texcoord : TEXCOORD;"
    ""
    "  row_major float4x4 World : WORLD;"
    "  float4 Color : COLOR;"
    "};"
    ""
    "struct PS_IN"
    "{"
    "  float4 Position : SV_POSITION;"
    "  float4 Color : COLOR;"
    "  float2 Texcoord : TEXCOORD;"
    "};"
    ""
    "cbuffer ConstantBuffer : register(b0)"
    "{"
    "  row_major float4x4 ViewProjectionMatrix;"
    "};"
    ""
    "PS_IN VSMain(VS_IN In)"
    "{"
    "  PS_IN Out;"
    ""
    "  const float4 World = mul(In.Position, In.World);"
    ""
    "  Out.Position = mul(World, ViewProjectionMatrix);"
    "  Out.Texcoord = In.Texcoord;"
    "  Out.Color = In.Color;"
    ""
    "  return Out;"
    "}"
    ""
    "Texture2D Diffuse : register(t0);"
    "Texture2D NormalMap : register(t1);"
    "SamplerState Sampler : register(s0);"
    ""
    "float4 PSMain(PS_IN In) : SV_TARGET"
    "{"
    "  return Diffuse.Sample(Sampler, In.Texcoord) * In.Color;"
    "}"
  };

  constexpr const char kDefaultSkinnedMeshShaderCode[] = {
    "struct VS_IN"
    "{"
    "  float4 Position : POSITION;"
    "  float3 Normal : NORMAL;"
    "  float3 Tangent : TANGENT;"
    "  float2 Texcoord : TEXCOORD;"
    "  float4 BoneWeights : WEIGHTS;"
    "  uint4 BoneIndices : BONES;"
    ""
    "  row_major float4x4 World : WORLD;"
    "  float4 Color : COLOR;"
    "};"
    ""
    "struct PS_IN"
    "{"
    "  float4 Position : SV_POSITION;"
    "  float4 Color : COLOR;"
    "  float2 Texcoord : TEXCOORD;"
    "};"
    ""
    "cbuffer ConstantBuffer : register(b0)"
    "{"
    "  row_major float4x4 ViewProjectionMatrix;"
    "};"
    ""
    "PS_IN VSMain(VS_IN In)"
    "{"
    "  PS_IN Out;"
    ""
    "  const float4 World = mul(In.Position, In.World);"
    ""
    "  Out.Position = mul(World, ViewProjectionMatrix);"
    "  Out.Texcoord = In.Texcoord;"
    "  Out.Color = In.Color;"
    ""
    "  return Out;"
    "}"
    ""
    "Texture2D Diffuse : register(t0);"
    "Texture2D NormalMap : register(t1);"
    "SamplerState Sampler : register(s0);"
    ""
    "float4 PSMain(PS_IN In) : SV_TARGET"
    "{"
    "  return Diffuse.Sample(Sampler, In.Texcoord) * In.Color;"
    "}"
  };

  constexpr const char* kDefaultShaderEntryPointVS = "VSMain";

  constexpr const char* kDefaultShaderEntryPointPS = "PSMain";
}
