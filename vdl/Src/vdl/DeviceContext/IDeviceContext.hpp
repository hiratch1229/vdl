#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Types.hpp>

class IDeviceContext
{
public:
  IDeviceContext() = default;

  virtual ~IDeviceContext() = default;

  virtual void SetScissor(const vdl::Scissor& _Scissor) = 0;

  virtual void SetViewport(const vdl::Viewport& _Viewport) = 0;

  virtual void SetBlendState(const vdl::BlendState& _BlendState) = 0;

  virtual void SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState) = 0;

  virtual void SetRasterizerState(const vdl::RasterizerState& _RasterizerState) = 0;

  //  TODO:マルチレンダーターゲット
  virtual void SetRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::DepthStencilTexture& _DepthStenilTexture) = 0;

  virtual void VSSetSampler(vdl::uint _StartSlot, vdl::uint _SamplerNum, vdl::SamplerState _Samplers[]) = 0;

  virtual void VSSetTexture(vdl::uint _StartSlot, vdl::uint _TextureNum, vdl::Texture* _Textures[]) = 0;

  virtual void HSSetSampler(vdl::uint _StartSlot, vdl::uint _SamplerNum, vdl::SamplerState _Samplers[]) = 0;

  virtual void HSSetTexture(vdl::uint _StartSlot, vdl::uint _TextureNum, vdl::Texture* _Textures[]) = 0;

  virtual void DSSetSampler(vdl::uint _StartSlot, vdl::uint _SamplerNum, vdl::SamplerState _Samplers[]) = 0;

  virtual void DSSetTexture(vdl::uint _StartSlot, vdl::uint _TextureNum, vdl::Texture* _Textures[]) = 0;

  virtual void GSSetSampler(vdl::uint _StartSlot, vdl::uint _SamplerNum, vdl::SamplerState _Samplers[]) = 0;

  virtual void GSSetTexture(vdl::uint _StartSlot, vdl::uint _TextureNum, vdl::Texture* _Textures[]) = 0;

  virtual void PSSetSampler(vdl::uint _StartSlot, vdl::uint _SamplerNum, vdl::SamplerState _Samplers[]) = 0;

  virtual void PSSetTexture(vdl::uint _StartSlot, vdl::uint _TextureNum, vdl::Texture* _Textures[]) = 0;

  //virtual void CSSetSampler(vdl::uint _StartSlot, vdl::uint _SamplerNum, vdl::SamplerState _Samplers[]) = 0;

  //virtual void CSSetTexture(vdl::uint _StartSlot, vdl::uint _TextureNum, vdl::Texture* _Textures[]) = 0;
  
  virtual void ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor) = 0;

  virtual void ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil) = 0;

  //virtual void ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::ColorF& _ClearColor) = 0;

  virtual void Draw(vdl::uint _VertexCount, vdl::uint _InstanceCount, vdl::uint _FirstVertex, vdl::uint _FirstInstance) = 0;

  virtual void DrawIndexed(vdl::uint _IndexCount, vdl::uint _InstanceCount, vdl::uint _FirstIndex, vdl::uint _VertexOffset, vdl::uint _FirstInstance) = 0;

  //virtual void Dispatch(vdl::uint _ThreadGropuX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ) = 0;
};
