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

  constexpr uint kMaxSamplerNum = 16;

  constexpr uint kMaxShaderResourceNum = 128;

  constexpr uint kMaxConstantBufferNum = 14;

  constexpr uint kMaxUnorderedAccessObjectNum = 8;

  constexpr const char kDefaultTextureVertexShaderCode[] = {
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
    "PS_IN main(VS_IN In)"
    "{"
    "PS_IN Out;"
    ""
    "Out.Position = mul(In.Position, In.NDCTransform);"
    "Out.Color = In.Color;"
    "Out.Texcoord = In.Texcoord * In.TexcoordScale + In.TexcoordTranslate;"
    ""
    "return Out;"
    "}" };

  constexpr const char kDefaultTexturePixelShaderCode[] = {
    "struct PS_IN"
    "{"
    "  float4 Position : SV_POSITION;"
    "  float4 Color : COLOR;"
    "  float2 Texcoord : TEXCOORD;"
    "};"
    ""
    "SamplerState Sampler : register(s0);"
    "Texture2D Texture : register(t0);"
    ""
    "float4 main(PS_IN In) : SV_TARGET"
    "{"
    "  return Texture.Sample(Sampler, In.Texcoord) * In.Color;"
    "}" };

  constexpr const char kDefaultStaticMeshVertexShaderCode[] = {
    "struct VS_IN"
    "{"
    "  float4 Position : POSITION;"
    "  float4 Normal : NORMAL;"
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
    "cbuffer CONSTANT_BUFFER : register(b0)"
    "{"
    "  row_major float4x4 ViewProjectionMatrix;"
    "};"
    ""
    "PS_IN main(VS_IN In)"
    "{"
    "  PS_IN Out;"
    ""
    "  const float4 World = mul(In.Position, In.World);"
    ""
    "  Out.Position = mul(World, ViewProjectionMatrix);"
    "  Out.Color = In.Color;"
    "  Out.Texcoord = In.Texcoord;"
    ""
    "  return Out;"
    "}" };

  constexpr const char kDefaultStaticMeshPixelShaderCode[] = {
    "struct PS_IN"
    "{"
    "  float4 Position : SV_POSITION;"
    "  float4 Color : COLOR;"
    "  float2 Texcoord : TEXCOORD;"
    "};"
    ""
    "SamplerState Sampler : register(s0);"
    "Texture2D Texture : register(t0);"
    ""
    "float4 main(PS_IN In) : SV_TARGET"
    "{"
    "  return Texture.Sample(Sampler, In.Texcoord) * In.Color;"
    "}" };

  constexpr const char kDefaultSkinnedMeshVertexShaderCode[] = {
    "struct VS_IN"
    "{"
    "  float4 Position : POSITION;"
    "  float4 Normal : NORMAL;"
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
    "cbuffer CONSTANT_BUFFER : register(b0)"
    "{"
    "  row_major float4x4 ViewProjectionMatrix;"
    "  row_major float4x4 BoneTransforms[32];"
    "};"
    ""
    "PS_IN main(VS_IN In)"
    "{"
    "  float3 Position = float3(0.0f, 0.0f, 0.0f);"
    ""
    "  for (int i = 0; i < 4; ++i)"
    "  {"
    "    Position += (mul(In.Position, BoneTransforms[In.BoneIndices[i]]) * In.BoneWeights[i]).xyz;"
    "  }"
    ""
    "  In.Position = float4(Position, 1.0f);"
    ""
    "  PS_IN Out;"
    ""
    "  const float4 World = mul(In.Position, In.World);"
    ""
    "  Out.Position = mul(World, ViewProjectionMatrix);"
    "  Out.Texcoord = In.Texcoord;"
    "  Out.Color = In.Color;"
    ""
    "  return Out;"
    "}" };

  constexpr const char kDefaultSkinnedMeshPixelShaderCode[] = {
    "struct PS_IN"
    "{"
    "  float4 Position : SV_POSITION;"
    "  float4 Color : COLOR;"
    "  float2 Texcoord : TEXCOORD;"
    "};"
    ""
    "SamplerState Sampler : register(s0);"
    "Texture2D Texture : register(t0);"
    ""
    "float4 main(PS_IN In) : SV_TARGET"
    "{"
    "  return Texture.Sample(Sampler, In.Texcoord) * In.Color;"
    "}" };
}
