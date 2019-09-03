#pragma once
#include "../IDeviceContext.hpp"

class CDeviceContext : public IDeviceContext
{
public:
  void Initialize()override;

  void SetInputLayout(vdl::RenderType _RenderType)override
  {

  }

  void SetVertexBuffer(const IBuffer* _pVertexBuffer)override
  {

  }

  void SetIndexBuffer(const IBuffer* _pIndexBuffer)override
  {

  }

  void SetInstanceBuffer(const IBuffer* _pInstanceBuffer)override
  {

  }

  void SetScissor(const vdl::Scissor& _Scissor)override
  {

  }

  void SetViewport(const vdl::Viewport& _Viewport)override
  {

  }

  void SetBlendState(const vdl::BlendState& _BlendState)override
  {

  }

  void SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState)override
  {

  }

  void SetRasterizerState(const vdl::RasterizerState& _RasterizerState)override
  {

  }

  void SetRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::DepthStencilTexture& _DepthStenilTexture)override
  {

  }

  void VSSetShader(const vdl::VertexShader& _VertexShader)override
  {

  }

  void VSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::SamplerState _Samplers[])override
  {

  }

  void VSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])override
  {

  }

  void VSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::detail::ConstantBufferData _ConstantBuffers[])override
  {
  
  }

  void HSSetShader(const vdl::HullShader& _VertexShader)override
  {

  }

  void HSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::SamplerState _Samplers[])override
  {

  }

  void HSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])override
  {

  }

  void HSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::detail::ConstantBufferData _ConstantBuffers[])override
  {

  }

  void DSSetShader(const vdl::DomainShader& _DomainShader)override
  {

  }

  void DSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::SamplerState _Samplers[])override
  {

  }

  void DSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])override
  {

  }

  void DSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::detail::ConstantBufferData _ConstantBuffers[])override
  {

  }

  void GSSetShader(const vdl::GeometryShader& _GeometryShader)override
  {

  }

  void GSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::SamplerState _Samplers[])override
  {

  }

  void GSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])override
  {

  }

  void GSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::detail::ConstantBufferData _ConstantBuffers[])override
  {

  }

  void PSSetShader(const vdl::PixelShader& _PixelShader)override
  {

  }

  void PSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, vdl::SamplerState _Samplers[])override
  {

  }

  void PSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, vdl::Texture _Textures[])override
  {

  }

  void PSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::detail::ConstantBufferData _ConstantBuffers[])override
  {

  }

  void ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)override;

  void ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)override;

  void Draw(vdl::uint _VertexCount, vdl::uint _InstanceCount, vdl::uint _FirstVertex, vdl::uint _FirstInstance)override;

  void DrawIndexed(vdl::uint _IndexCount, vdl::uint _InstanceCount, vdl::uint _FirstIndex, vdl::uint _VertexOffset, vdl::uint _FirstInstance)override;
};
