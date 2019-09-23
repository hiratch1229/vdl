#include "CDeviceContext.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/DirectX11/CDevice.hpp>

#include <vdl/Buffer/DirectX11/CBuffer.hpp>
#include <vdl/Texture/DirectX11/CTexture.hpp>
#include <vdl/Shader/DirectX11/CShader.hpp>

#include <assert.h>

void CDeviceContext::Initialize()
{
  pImmediateContext_ = static_cast<CDevice*>(Engine::Get<IDevice>())->GetImmediateContext();
}

void CDeviceContext::SetVertexBuffer(const IBuffer* _pVertexBuffer)
{
  assert(_pVertexBuffer->GetType() == BufferType::eVertexBuffer);

  constexpr vdl::uint kOffset = 0;

  const CVertexBuffer* pVertexBuffer = static_cast<const CVertexBuffer*>(_pVertexBuffer);

  pImmediateContext_->IASetVertexBuffers(0, 1, pVertexBuffer->pBuffer.GetAddressOf(), &pVertexBuffer->Stride, &kOffset);
}

void CDeviceContext::SetInstanceBuffer(const IBuffer* _pInstanceBuffer)
{
  assert(_pInstanceBuffer->GetType() == BufferType::eInstanceBuffer);

  constexpr vdl::uint kOffset = 0;

  const CInstanceBuffer* pInstanceBuffer = static_cast<const CInstanceBuffer*>(_pInstanceBuffer);

  pImmediateContext_->IASetVertexBuffers(1, 1, pInstanceBuffer->pBuffer.GetAddressOf(), &pInstanceBuffer->Stride, &kOffset);
}

void CDeviceContext::SetIndexBuffer(const IBuffer* _pIndexBuffer)
{
  assert(_pIndexBuffer->GetType() == BufferType::eIndexBuffer);

  constexpr vdl::uint kOffset = 0;

  const CIndexBuffer* pIndexBuffer = static_cast<const CIndexBuffer*>(_pIndexBuffer);

  pImmediateContext_->IASetIndexBuffer(pIndexBuffer->pBuffer.Get(), (pIndexBuffer->IndexType == IndexType::eUint16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT), kOffset);
}

void CDeviceContext::SetInputLayout(vdl::InputLayout _InputLayout)
{

}

void CDeviceContext::SetTopology(vdl::Topology _Topology)
{

}

void CDeviceContext::SetScissor(const vdl::Scissor& _Scissor)
{

}

void CDeviceContext::SetViewport(const vdl::Viewport& _Viewport)
{

}

void CDeviceContext::CDeviceContext::SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStenilTexture)
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

void CDeviceContext::VSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])
{

}

void CDeviceContext::VSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{

}

void CDeviceContext::VSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{

}

void CDeviceContext::HSSetShader(const vdl::HullShader& _VertexShader)
{

}

void CDeviceContext::HSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])
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

void CDeviceContext::DSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])
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

void CDeviceContext::GSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])
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

void CDeviceContext::PSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])
{

}

void CDeviceContext::PSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{

}

void CDeviceContext::PSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{

}

void CDeviceContext::ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)
{

}

void CDeviceContext::ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)
{

}

void CDeviceContext::Draw(vdl::uint _VertexCount, vdl::uint _InstanceCount, vdl::uint _FirstVertex, vdl::uint _FirstInstance)
{

}

void CDeviceContext::DrawIndexed(vdl::uint _IndexCount, vdl::uint _InstanceCount, vdl::uint _FirstIndex, vdl::uint _VertexOffset, vdl::uint _FirstInstance)
{

}

void CDeviceContext::Flush()
{

}
