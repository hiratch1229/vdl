#pragma once
#include "../IDeviceContext.hpp"

#include <vdl/InputLayout.hpp>
#include <vdl/GraphicsState.hpp>
#include <vdl/Sampler.hpp>

#include <d3d11.h>
#include <wrl.h>

#include <map>
#include <unordered_map>

class ITextureManager;
class IBufferManager;
class IShaderManager;

class CDeviceContext : public IDeviceContext
{
  ITextureManager* pTextureManager_;
  IBufferManager* pBufferManager_;
  IShaderManager* pShaderManager_;
private:
  ID3D11Device* pDevice_;
  ID3D11DeviceContext* pImmediateContext_;
  ID3D11RenderTargetView* pSwapChainRenderTargetView_;
  ID3D11DepthStencilView* pSwapChainDepthStencilView_;
private:
  std::unordered_map<vdl::InputLayout, Microsoft::WRL::ComPtr<ID3D11InputLayout>> InputLayouts_;
  std::map<vdl::BlendState, Microsoft::WRL::ComPtr<ID3D11BlendState>> BlendStates_;
  std::unordered_map<vdl::DepthStencilState, Microsoft::WRL::ComPtr<ID3D11DepthStencilState>> DepthStencilStates_;
  std::unordered_map<vdl::RasterizerState, Microsoft::WRL::ComPtr<ID3D11RasterizerState>> RasterizerStates_;
  std::unordered_map<vdl::Sampler, Microsoft::WRL::ComPtr<ID3D11SamplerState>> Samplers_;
public:
  [[nodiscard]] bool isFoundInputLayout(vdl::InputLayout _InputLayout) { return InputLayouts_.find(_InputLayout) == InputLayouts_.end(); }
  void RegisterInputLayout(vdl::InputLayout _Key, ID3DBlob* _pCode);
  ID3D11SamplerState* GetSamplerState(const vdl::Sampler& _Sampler);
public:
  CDeviceContext() = default;

  void Initialize()override;

  void SetVertexBuffer(const IBuffer* _pVertexBuffer)override;

  void SetInstanceBuffer(const IBuffer* _pInstanceBuffer)override;

  void SetIndexBuffer(const IBuffer* _pIndexBuffer)override;

  void SetInputLayout(vdl::InputLayout _InputLayout)override;

  void SetTopology(vdl::Topology _Topology)override;

  void SetScissor(const vdl::Scissor& _Scissor)override;

  void SetViewport(const vdl::Viewport& _Viewport)override;

  void SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStenilTexture)override;

  void SetBlendState(const vdl::BlendState& _BlendState)override;

  void SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState)override;

  void SetRasterizerState(const vdl::RasterizerState& _RasterizerState)override;

  void VSSetShader(const vdl::VertexShader& _VertexShader)override;

  void VSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])override;

  void VSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void VSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void HSSetShader(const vdl::HullShader& __HullShader)override;

  void HSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])override;

  void HSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void HSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void DSSetShader(const vdl::DomainShader& _DomainShader)override;

  void DSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])override;

  void DSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void DSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void GSSetShader(const vdl::GeometryShader& _GeometryShader)override;

  void GSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])override;

  void GSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void GSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void PSSetShader(const vdl::PixelShader& _PixelShader)override;

  void PSSetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])override;

  void PSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void PSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)override;

  void ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)override;

  void Draw(vdl::uint _VertexCount, vdl::uint _InstanceCount, vdl::uint _FirstVertex, vdl::uint _FirstInstance)override;

  void DrawIndexed(vdl::uint _IndexCount, vdl::uint _InstanceCount, vdl::uint _FirstIndex, vdl::uint _VertexOffset, vdl::uint _FirstInstance)override;

  void Flush()override {};
};
