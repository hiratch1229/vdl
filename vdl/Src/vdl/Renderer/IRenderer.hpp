#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Types.hpp>

#include <vdl/Shader/IShader.hpp>

#include <vector>

enum class RenderType
{
  eNone,
  eTexture,
  eStaticMesh,
  eSkinnedMesh,

  eNum
};

class IRenderer
{
public:
  static IRenderer* Create();

  IRenderer() = default;

  virtual ~IRenderer() = default;

  virtual void Initialize() = 0;

  virtual void SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStencilTexture) = 0;

  virtual void SetScissor(const vdl::Scissor& _Scissor, RenderType _Type) = 0;

  virtual void SetViewport(const vdl::Viewport& _Viewport, RenderType _Type) = 0;

  virtual void SetBlendState(const vdl::BlendState& _BlendState, RenderType _Type) = 0;

  virtual void SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState, RenderType _Type) = 0;

  virtual void SetRasterizerState(const vdl::RasterizerState& _RasterizerState, RenderType _Type) = 0;

  virtual void SetVertexShader(const vdl::VertexShader& _VertexShader, RenderType _Type) = 0;

  virtual void SetHullShader(const vdl::HullShader& _HullShader, RenderType _Type) = 0;

  virtual void SetDomainShader(const vdl::DomainShader& _DomainShader, RenderType _Type) = 0;

  virtual void SetGeometryShader(const vdl::GeometryShader& _GeometryShader, RenderType _Type) = 0;

  virtual void SetPixelShader(const vdl::PixelShader& _PixelShader, RenderType _Type) = 0;

  virtual void SetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[], ShaderType _Stage, RenderType _Type) = 0;

  virtual void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[], ShaderType _Stage, RenderType _Type) = 0;

  virtual void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _BufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[], ShaderType _Stage, RenderType _Type) = 0;

  virtual void Draw(const vdl::Texture& _Texture, const vdl::float2& _DstLeftTop, const vdl::float2& _DstSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::ColorF& _Color) = 0;

  virtual void Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::ColorF& _Color) = 0;

  virtual void Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const vdl::MotionBlendDatas& _MotionBlendDatas, const vdl::ColorF& _Color) = 0;

  virtual void Clear(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor) = 0;

  virtual void Clear(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil) = 0;

  virtual void Flush() = 0;

  //virtual void SetComputeShader(const vdl::ComputeShader& _ComputeShader) = 0;
};
