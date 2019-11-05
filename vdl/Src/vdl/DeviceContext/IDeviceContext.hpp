#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Types.hpp>
#include <vdl/Matrix.hpp>

#include <vdl/Buffer/Buffer.hpp>

class IDeviceContext
{
public:
  static IDeviceContext* Create();

  IDeviceContext() = default;

  virtual ~IDeviceContext() = default;

  virtual void Initialize() = 0;

  virtual void SetVertexBuffer(const VertexBuffer& _VertexBuffer) = 0;

  virtual void SetInstanceBuffer(const InstanceBuffer& _InstanceBuffer) = 0;

  virtual void SetIndexBuffer(const IndexBuffer& _IndexBuffer) = 0;

  virtual void SetInputLayout(vdl::InputLayoutType _InputLayout) = 0;

  virtual void SetTopology(vdl::TopologyType _Topology) = 0;

  virtual void SetScissor(const vdl::Scissor& _Scissor) = 0;

  virtual void SetViewport(const vdl::Viewport& _Viewport) = 0;

  virtual void SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStencilTexture) = 0;

  virtual void SetBlendState(const vdl::BlendState& _BlendState) = 0;

  virtual void SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState) = 0;

  virtual void SetRasterizerState(const vdl::RasterizerState& _RasterizerState) = 0;

  virtual void VSSetShader(const vdl::VertexShader& _VertexShader) = 0;

  virtual void VSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]) = 0;

  virtual void VSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]) = 0;

  virtual void VSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]) = 0;

  virtual void HSSetShader(const vdl::HullShader& _HullShader) = 0;

  virtual void HSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]) = 0;

  virtual void HSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]) = 0;

  virtual void HSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]) = 0;

  virtual void DSSetShader(const vdl::DomainShader& _DomainShader) = 0;

  virtual void DSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]) = 0;

  virtual void DSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]) = 0;

  virtual void DSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]) = 0;

  virtual void GSSetShader(const vdl::GeometryShader& _GeometryShader) = 0;

  virtual void GSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]) = 0;

  virtual void GSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]) = 0;

  virtual void GSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]) = 0;

  virtual void PSSetShader(const vdl::PixelShader& _PixelShader) = 0;

  virtual void PSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]) = 0;

  virtual void PSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]) = 0;

  virtual void PSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]) = 0;

  virtual void CSSetShader(const vdl::ComputeShader& _ComputeShader) = 0;

  virtual void CSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]) = 0;

  virtual void CSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]) = 0;

  virtual void CSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]) = 0;

  virtual void CSSetUnorderedAccessObjects(vdl::uint _StartSlot, vdl::uint _UnorderedAccessObjectNum, const vdl::UnorderedAccessObject _UnorderedAccessObjects[]) = 0;

  virtual void ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor) = 0;

  virtual void ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil) = 0;

  virtual void ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::ColorF& _ClearColor) = 0;

  virtual void Draw(vdl::uint _VertexCount, vdl::uint _InstanceCount, vdl::uint _FirstVertex, vdl::uint _FirstInstance) = 0;

  virtual void DrawIndexed(vdl::uint _IndexCount, vdl::uint _InstanceCount, vdl::uint _FirstIndex, vdl::uint _VertexOffset, vdl::uint _FirstInstance) = 0;

  virtual void Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ) = 0;

  virtual void Flush() = 0;
};
