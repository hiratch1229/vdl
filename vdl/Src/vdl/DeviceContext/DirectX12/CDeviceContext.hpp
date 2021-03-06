#pragma once
#include "../IDeviceContext.hpp"

#include <vdl/InputLayout.hpp>
#include <vdl/Scissor.hpp>
#include <vdl/Viewport.hpp>
#include <vdl/GraphicsStates.hpp>
#include <vdl/Shader.hpp>
#include <vdl/Texture.hpp>
#include <vdl/Sampler.hpp>
#include <vdl/ConstantBuffer.hpp>
#include <vdl/UnorderedAccessBuffer.hpp>
#include <vdl/Hash.hpp>
#include <vdl/Variant.hpp>

#include <vdl/CommandList/GraphicsCommandList/GraphicsCommandList.hpp>
#include <vdl/CommandList/ComputeCommandList/ComputeCommandList.hpp>
#include <vdl/Constants/Constants.hpp>

#include <vdl/pch/DirectX12/pch.hpp>

#include "SwapChain/SwapChain.hpp"
#include <vdl/Device/DirectX12/CommandList/CommandList.hpp>
#include <vdl/Device/DirectX12/DescriptorHeap/DescriptorHeap.hpp>
#include <vdl/Device/DirectX12/Fence/Fence.hpp>
#include <vdl/Texture/DirectX12/CTexture.hpp>
#include <vdl/Shader/DirectX12/CShader.hpp>

#include <array>
#include <unordered_map>

class CDevice;
class IWindow;
class ITextureManager;
class IBufferManager;
class IShaderManager;
class IRenderer;

class CDeviceContext : public IDeviceContext
{
  static constexpr DescriptorHeapType kDescriptorHeapTypes[] = {
    DescriptorHeapType::eCBV_SRV_UAV,
    DescriptorHeapType::eSampler,
  };
  static constexpr vdl::uint kDescriptorHeapNum = static_cast<vdl::uint>(vdl::Macro::ArraySize(kDescriptorHeapTypes));
private:
  using Texture = vdl::Variant<vdl::Texture, vdl::DepthStencilTexture>;
  using ShaderResources = std::vector<vdl::ShaderResource>;
  using Samplers = std::vector<vdl::Sampler>;
  using ConstantBuffers = std::vector<vdl::Detail::ConstantBufferData>;
  using UnorderedAccessObjects = std::vector<vdl::UnorderedAccessObject>;
private:
  struct GraphicsReserveData
  {
    std::vector<vdl::OutputManager> OutputManagers;

    std::vector<DescriptorHeap> ShaderResourceHeaps;
    std::vector<DescriptorHeap> SamplerHeaps;
    std::vector<DescriptorHeap> ConstantBufferHeaps;

    std::unordered_map<vdl::ID, Texture> ClearTextures;

    std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> PipelineStates;
  public:
    GraphicsReserveData() = default;

    void Clear()
    {
      OutputManagers.clear();

      ShaderResourceHeaps.clear();
      SamplerHeaps.clear();
      ConstantBufferHeaps.clear();

      ClearTextures.clear();

      PipelineStates.clear();
    }
  };
  struct ComputeReserveData
  {
    std::vector<DescriptorHeap> ShaderResourceHeaps;
    std::vector<DescriptorHeap> SamplerHeaps;
    std::vector<DescriptorHeap> ConstantBufferHeaps;
    std::vector<DescriptorHeap> UnorderedAccessHeaps;

    std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> PipelineStates;
  public:
    ComputeReserveData() = default;

    void Clear()
    {
      ShaderResourceHeaps.clear();
      SamplerHeaps.clear();
      ConstantBufferHeaps.clear();
      UnorderedAccessHeaps.clear();

      PipelineStates.clear();
    }
  };
private:
  ID3D12Device5* pD3D12Device_;
  SwapChain SwapChain_;
  vdl::uint CurrentBackBufferIndex_;
private:
  CDevice* pDevice_;
  IWindow* pWindow_;
  ITextureManager* pTextureManager_;
  IBufferManager* pBufferManager_;
  IShaderManager* pShaderManager_;
  IRenderer* pRenderer_;
private:
  std::unordered_map<vdl::InputLayoutType, std::vector<D3D12_INPUT_ELEMENT_DESC>> InputLayouts_;
  std::unordered_map<vdl::BlendState, D3D12_BLEND_DESC> BlendStates_;
  std::unordered_map<vdl::RasterizerState, D3D12_RASTERIZER_DESC> RasterizerStates_;
  std::unordered_map<vdl::DepthStencilState, D3D12_DEPTH_STENCIL_DESC> DepthStencilStates_;
  std::unordered_map<vdl::Sampler, DescriptorHeap> Samplers_;
  DescriptorAllocator* pSamplerDescriptorAllocator_;
  std::array<DescriptorAllocator, kDescriptorHeapNum> DescriptorAllocators_;
  std::array<ID3D12DescriptorHeap*, kDescriptorHeapNum> ppDescriptorHeaps_;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pDefaultViewDescriptorHeap_;
  D3D12_CPU_DESCRIPTOR_HANDLE DefaultViewCPUHandle_;
  HANDLE FenceEvent_;
  Fence* pLastFence_ = nullptr;
  vdl::InputLayoutType CurrentInputLayoutType_ = vdl::InputLayoutType::eNone;
private:
  ID3D12CommandQueue* pGraphicsCommandQueue_;
  Microsoft::WRL::ComPtr<ID3D12RootSignature> pGraphicsRootSignature_;
  std::array<CommandList, Constants::kGraphicsCommandBufferNum> GraphicsCommandLists_;
  vdl::uint GraphicsCommandBufferIndex_ = 0;
  bool isChangeGraphicsPipelineState_ = true;
  D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsPipelineStateDesc_;
  std::array<Fence, Constants::kGraphicsCommandBufferNum> GraphicsFences_;
  std::array<BaseGraphicsCommandList, Constants::kGraphicsCommandBufferNum> GraphicsCommandListDatas_;
  std::array<GraphicsReserveData, Constants::kGraphicsCommandBufferNum> GraphicsReserveDatas_;
private:
  ID3D12CommandQueue* pComputeCommandQueue_;
  Microsoft::WRL::ComPtr<ID3D12RootSignature> pComputeRootSignature_;
  std::array<CommandList, Constants::kComputeCommandBufferNum> ComputeCommandLists_;
  vdl::uint ComputeCommandBufferIndex_ = 0;
  D3D12_COMPUTE_PIPELINE_STATE_DESC ComputePipelineStateDesc_;
  std::array<Fence, Constants::kComputeCommandBufferNum> ComputeFences_;
  std::array<ComputeCommandList, Constants::kComputeCommandBufferNum> ComputeCommandListDatas_;
  std::array<ComputeReserveData, Constants::kComputeCommandBufferNum> ComputeReserveDatas_;
private:
  GraphicsReserveData& GetCurrentGraphicsReserveData() { return GraphicsReserveDatas_[GraphicsCommandBufferIndex_]; }
  CommandList& GetCurrentGraphicsCommandList() { return GraphicsCommandLists_[GraphicsCommandBufferIndex_]; }
  Fence& GetCurrentGraphicsFence() { return GraphicsFences_[GraphicsCommandBufferIndex_]; }
  ComputeReserveData& GetCurrentComputeReserveData() { return ComputeReserveDatas_[ComputeCommandBufferIndex_]; }
  CommandList& GetCurrentComputeCommandList() { return ComputeCommandLists_[ComputeCommandBufferIndex_]; }
  Fence& GetCurrentComputeFence() { return ComputeFences_[ComputeCommandBufferIndex_]; }
private:
  CRenderTexture* GetD3D12RenderTexture(const vdl::RenderTexture& _RenderTexture);
  const D3D12_BLEND_DESC& GetBlendDesc(const vdl::BlendState& _BlendState);
  const D3D12_DEPTH_STENCIL_DESC& GetDepthStecilDesc(const vdl::DepthStencilState& _DepthStencilState);
  const D3D12_RASTERIZER_DESC& GetRasterizerDesc(const vdl::RasterizerState& _RasterizerState);
  const D3D12_CPU_DESCRIPTOR_HANDLE& GetShaderResourceCPUHandle(const vdl::ShaderResource& _ShaderResource, CommandList* _pCommandList);
  const D3D12_CPU_DESCRIPTOR_HANDLE& GetSamplerCPUHandle(const vdl::Sampler& _Sampler);
  const D3D12_CPU_DESCRIPTOR_HANDLE& GetConstantBufferCPUHandle(const vdl::Detail::ConstantBufferData& _ConstantBuffer);
  const D3D12_CPU_DESCRIPTOR_HANDLE& GetUnorderedAccessObjectCPUHandle(const vdl::UnorderedAccessObject& _UnorderedAccessObject, CommandList* _pCommandList);
  vdl::uint GetVertexBufferStride()const;
  vdl::uint GetInstanceBufferStride()const;
public:
  CDeviceContext() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

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
