#pragma once
#include "../IDeviceContext.hpp"

#include <vdl/InputLayout.hpp>
#include <vdl/GraphicsStates.hpp>
#include <vdl/Texture.hpp>
#include <vdl/Sampler.hpp>
#include <vdl/Hash.hpp>
#include <vdl/Variant.hpp>

#include <vdl/pch/DirectX11/pch.hpp>
#include "SwapChain/SwapChain.hpp"

#include <unordered_map>

class IDevice;
class IWindow;
class ITextureManager;
class IBufferManager;
class IShaderManager;

class CDeviceContext : public IDeviceContext
{
  ID3D11Device* pD3D11Device_;
  ID3D11DeviceContext* pD3D11ImmediateContext_;
  SwapChain SwapChain_;
private:
  IDevice* pDevice_;
  IWindow* pWindow_;
  ITextureManager* pTextureManager_;
  IBufferManager* pBufferManager_;
  IShaderManager* pShaderManager_;
private:
  vdl::InputLayoutType CurrentInputLayoutType_ = vdl::InputLayoutType::eNone;
private:
  std::unordered_map<vdl::InputLayoutType, Microsoft::WRL::ComPtr<ID3D11InputLayout>> InputLayouts_;
  std::unordered_map<vdl::BlendState, Microsoft::WRL::ComPtr<ID3D11BlendState>> BlendStates_;
  std::unordered_map<vdl::DepthStencilState, Microsoft::WRL::ComPtr<ID3D11DepthStencilState>> DepthStencilStates_;
  std::unordered_map<vdl::RasterizerState, Microsoft::WRL::ComPtr<ID3D11RasterizerState>> RasterizerStates_;
  std::unordered_map<vdl::Sampler, Microsoft::WRL::ComPtr<ID3D11SamplerState>> Samplers_;
public:
  [[nodiscard]] bool isFoundInputLayout(vdl::InputLayoutType _InputLayout) { return InputLayouts_.find(_InputLayout) != InputLayouts_.end(); }
  void RegisterInputLayout(vdl::InputLayoutType _Key, ID3DBlob* _pCode);
private:
  ID3D11BlendState* GetBlendState(const vdl::BlendState& _BlendState);
  ID3D11DepthStencilState* GetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState);
  ID3D11RasterizerState* GetRasterizerState(const vdl::RasterizerState& _RasterizerState);
  ID3D11RenderTargetView* GetRenderTargetView(const vdl::RenderTexture& _RenderTexture);
  ID3D11ShaderResourceView* GetShaderResourceView(const vdl::ShaderResource& _ShaderResource);
  ID3D11SamplerState* GetSamplerState(const vdl::Sampler& _Sampler);
  ID3D11Buffer* GetConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer);
  ID3D11UnorderedAccessView* GetUnorderedAccessView(const vdl::UnorderedAccessObject& _UnorderedAccessObject);
  vdl::uint GetVertexBufferStride()const;
  vdl::uint GetInstanceBufferStride()const;
public:
  CDeviceContext() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX11; }

  void Initialize()override;

  void SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStencilTexture)override;

  void ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::Color4F& _ClearColor)override;

  void ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)override;

  void ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::Color4F& _ClearColor)override;

  void Execute(const BaseGraphicsCommandList& _GraphicsCommandList)override;

  void Execute(const ComputeCommandList& _ComputeCommandList)override;

  void Flush()override;

  void Present()override;

  void ChangeWindowMode()override;

  void ScreenShot()override;

  const vdl::RenderTexture& GetRenderTexture()const override { return SwapChain_.GetRenderTexture(); }

  const vdl::DepthStencilTexture& GetDepthStencilTexture()const override { return SwapChain_.GetDepthStencilTexture(); }
};
