#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Types.hpp>

#include <vdl/Platform/Platform.hpp>

#include <vdl/Shader/IShader.hpp>

class IRenderer
{
public:
  static IRenderer* Create();

  IRenderer() = default;

  virtual ~IRenderer() = default;

  [[nodiscard]] virtual PlatformFlags GetPlatform()const = 0;

  virtual void Initialize() = 0;

  virtual vdl::Matrix GetView()const = 0;

  virtual vdl::Matrix GetProjection()const = 0;

  virtual void SetCamera(const vdl::Camera& _Camera) = 0;

  virtual void SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStencilTexture) = 0;

  virtual void SetTopology(vdl::TopologyType _Topology, vdl::InputLayoutType _Type) = 0;

  virtual void SetScissor(const vdl::Scissor& _Scissor, vdl::InputLayoutType _Type) = 0;

  virtual void SetViewport(const vdl::Viewport& _Viewport, vdl::InputLayoutType _Type) = 0;

  virtual void SetBlendState(const vdl::BlendState& _BlendState, vdl::InputLayoutType _Type) = 0;

  virtual void SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState, vdl::InputLayoutType _Type) = 0;

  virtual void SetRasterizerState(const vdl::RasterizerState& _RasterizerState, vdl::InputLayoutType _Type) = 0;

  virtual void SetVertexShader(const vdl::VertexShader& _VertexShader, vdl::InputLayoutType _Type) = 0;

  virtual void SetHullShader(const vdl::HullShader& _HullShader, vdl::InputLayoutType _Type) = 0;

  virtual void SetDomainShader(const vdl::DomainShader& _DomainShader, vdl::InputLayoutType _Type) = 0;

  virtual void SetGeometryShader(const vdl::GeometryShader& _GeometryShader, vdl::InputLayoutType _Type) = 0;

  virtual void SetPixelShader(const vdl::PixelShader& _PixelShader, vdl::InputLayoutType _Type) = 0;

  virtual void SetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[], ShaderType _Stage, vdl::InputLayoutType _Type) = 0;

  virtual void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[], ShaderType _Stage, vdl::InputLayoutType _Type) = 0;

  virtual void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[], ShaderType _Stage, vdl::InputLayoutType _Type) = 0;

  virtual void Draw(vdl::uint _VertexCount) = 0;

  virtual void Draw(const vdl::Texture& _Texture, const vdl::float2& _DstLeftTop, const vdl::float2& _DstSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::Color4F& _Color) = 0;

  virtual void Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::Color4F& _Color) = 0;

  virtual void Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const vdl::MotionBlendDatas& _MotionBlendDatas, const vdl::Color4F& _Color) = 0;

  virtual void Clear(const vdl::RenderTexture& _RenderTexture, const vdl::Color4F& _ClearColor) = 0;

  virtual void Clear(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil) = 0;

  virtual void Clear(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::Color4F& _ClearColor) = 0;

  virtual void Flush() = 0;
};
