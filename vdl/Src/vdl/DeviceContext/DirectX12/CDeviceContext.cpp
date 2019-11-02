#include "CDeviceContext.hpp"

void CDeviceContext::Initialize()
{

}

void CDeviceContext::SetVertexBuffer(const VertexBuffer& _VertexBuffer)
{

}

void CDeviceContext::SetInstanceBuffer(const InstanceBuffer& _InstanceBuffer)
{

}

void CDeviceContext::SetIndexBuffer(const IndexBuffer& _IndexBuffer)
{

}

void CDeviceContext::SetInputLayout(vdl::InputLayoutType _InputLayout)
{

}

void CDeviceContext::SetTopology(vdl::TopologyType _Topology)
{

}

void CDeviceContext::SetScissor(const vdl::Scissor& _Scissor)
{

}

void CDeviceContext::SetViewport(const vdl::Viewport& _Viewport)
{

}

void CDeviceContext::SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStenilTexture)
{

}

void CDeviceContext::SetBlendState(const vdl::BlendState& _BlendState)
{

}

void CDeviceContext::SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState)
{

}

void CDeviceContext::SetRasterizerState(const vdl::RasterizerState& _RasterizerState)
{

}

void CDeviceContext::VSSetShader(const vdl::VertexShader& _VertexShader)
{

}

void CDeviceContext::VSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{

}

void CDeviceContext::VSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{

}

void CDeviceContext::VSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{

}

void CDeviceContext::HSSetShader(const vdl::HullShader& _HullShader)
{

}

void CDeviceContext::HSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{

}

void CDeviceContext::HSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{

}

void CDeviceContext::HSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{

}

void CDeviceContext::DSSetShader(const vdl::DomainShader& _DomainShader)
{

}

void CDeviceContext::DSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{

}

void CDeviceContext::DSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{

}

void CDeviceContext::DSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{

}

void CDeviceContext::GSSetShader(const vdl::GeometryShader& _GeometryShader)
{

}

void CDeviceContext::GSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{

}

void CDeviceContext::GSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{

}

void CDeviceContext::GSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{

}

void CDeviceContext::PSSetShader(const vdl::PixelShader& _PixelShader)
{

}

void CDeviceContext::PSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{

}

void CDeviceContext::PSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{

}

void CDeviceContext::PSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{

}

void CDeviceContext::CSSetShader(const vdl::ComputeShader& _ComputeShader)
{

}

void CDeviceContext::CSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{

}

void CDeviceContext::CSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{

}

void CDeviceContext::CSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{

}

void CDeviceContext::CSSetUnorderedAccessObjects(vdl::uint _StartSlot, vdl::uint _UnorderedAccessObjectNum, const vdl::UnorderedAccessObject _UnorderedAccessObjects[])
{

}

void CDeviceContext::ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)
{

}

void CDeviceContext::ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)
{

}

void CDeviceContext::ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::ColorF& _ClearColor)
{

}

void CDeviceContext::Draw(vdl::uint _VertexCount, vdl::uint _InstanceCount, vdl::uint _FirstVertex, vdl::uint _FirstInstance)
{

}

void CDeviceContext::DrawIndexed(vdl::uint _IndexCount, vdl::uint _InstanceCount, vdl::uint _FirstIndex, vdl::uint _VertexOffset, vdl::uint _FirstInstance)
{

}

void CDeviceContext::Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)
{

}

void CDeviceContext::Flush()
{

}
