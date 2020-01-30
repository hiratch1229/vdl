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

#include <vdl/StateChangeFlags/StateChangeFlags.hpp>
#include <vdl/Constants/Constants.hpp>

#include <vdl/pch/DirectX12/pch.hpp>

#include <vdl/Device/DirectX12/CommandList/CommandList.hpp>
#include <vdl/Device/DirectX12/DescriptorHeap/DescriptorHeap.hpp>
#include <vdl/Device/DirectX12/Fence/Fence.hpp>
#include <vdl/Texture/DirectX12/CTexture.hpp>
#include <vdl/Shader/DirectX12/CShader.hpp>

#include <array>
#include <unordered_map>

class CDevice;
class CSwapChain;
class ITextureManager;
class IBufferManager;
class IShaderManager;

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
  enum class GraphicsCommandType
  {
    eSetVertexBuffer,
    eSetInstanceBuffer,
    eSetIndexBuffer,
    eSetInputLayout,
    eSetTopology,
    eSetScissor,
    eSetViewport,
    eSetRenderTextures,
    eSetBlendState,
    eSetDepthStencilState,
    eSetRasterizerState,
    eSetVertexShader,
    eSetHullShader,
    eSetDomainShader,
    eSetGeometryShader,
    eSetPixelShader,
    eSetVertexStageShaderResource,
    eSetHullStageShaderResource,
    eSetDomainStageShaderResource,
    eSetGeometryStageShaderResource,
    eSetPixelStageShaderResource,
    eSetVertexStageSampler,
    eSetHullStageSampler,
    eSetDomainStageSampler,
    eSetGeometryStageSampler,
    eSetPixelStageSampler,
    eSetVertexStageConstantBuffer,
    eSetHullStageConstantBuffer,
    eSetDomainStageConstantBuffer,
    eSetGeometryStageConstantBuffer,
    eSetPixelStageConstantBuffer,

    eNum
  };
  static constexpr vdl::uint kGraphicsCommandNum = static_cast<vdl::uint>(GraphicsCommandType::eNum);
  static_assert(kGraphicsCommandNum <= sizeof(vdl::uint32_t) * 8);
  static constexpr GraphicsCommandType kGraphicsPipelineStateCommands[] = {
    GraphicsCommandType::eSetInputLayout, GraphicsCommandType::eSetTopology, GraphicsCommandType::eSetRenderTextures, GraphicsCommandType::eSetBlendState, GraphicsCommandType::eSetDepthStencilState, GraphicsCommandType::eSetRasterizerState,
    GraphicsCommandType::eSetVertexShader, GraphicsCommandType::eSetHullShader, GraphicsCommandType::eSetDomainShader, GraphicsCommandType::eSetGeometryShader, GraphicsCommandType::eSetPixelShader };
  struct GraphicsState
  {
    VertexBuffer VertexBuffer;
    InstanceBuffer InstanceBuffer;
    IndexBuffer IndexBuffer;
    vdl::InputLayoutType InputLayout;
    vdl::TopologyType Topology;
    vdl::Scissor Scissor;
    vdl::Viewport Viewport;
    vdl::BlendState BlendState;
    vdl::DepthStencilState DepthStencilState;
    vdl::RasterizerState RasterizerState;
    vdl::VertexShader VertexShader;
    vdl::HullShader HullShader;
    vdl::DomainShader DomainShader;
    vdl::GeometryShader GeometryShader;
    vdl::PixelShader PixelShader;
    std::array<ShaderResources, kGraphicsShaderStageNum> ShaderResources;
    std::array<Samplers, kGraphicsShaderStageNum> Samplers;
    std::array<ConstantBuffers, kGraphicsShaderStageNum> ConstantBuffers;
  };
  struct GraphicsReserveData
  {
    std::vector<VertexBuffer> VertexBuffers;
    std::vector<InstanceBuffer> InstanceBuffers;
    std::vector<IndexBuffer> IndexBuffers;
    std::vector<vdl::OutputManager> OutputManagers;
    std::vector<vdl::VertexShader> VertexShaders;
    std::vector<vdl::HullShader> HullShaders;
    std::vector<vdl::DomainShader> DomainShaders;
    std::vector<vdl::GeometryShader> GeometryShaders;
    std::vector<vdl::PixelShader> PixelShaders;
    std::vector<ShaderResources> ShaderResources;
    std::vector<ConstantBuffers> ConstantBuffers;

    std::vector<DescriptorHeap> ShaderResourceHeaps;
    std::vector<DescriptorHeap> SamplerHeaps;
    std::vector<DescriptorHeap> ConstantBufferHeaps;

    std::unordered_map<vdl::ID, Texture> ClearTextures;

    std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> PipelineStates;
  public:
    GraphicsReserveData() = default;

    void Clear()
    {
      VertexBuffers.clear();
      InstanceBuffers.clear();
      IndexBuffers.clear();
      OutputManagers.clear();
      VertexShaders.clear();
      HullShaders.clear();
      DomainShaders.clear();
      GeometryShaders.clear();
      PixelShaders.clear();
      ShaderResources.clear();
      ConstantBuffers.clear();

      ShaderResourceHeaps.clear();
      SamplerHeaps.clear();
      ConstantBufferHeaps.clear();

      ClearTextures.clear();

      PipelineStates.clear();
    }
  };
private:
  enum class ComputeCommandType : vdl::uint8_t
  {
    eSetComputeShader,
    eSetShaderResource,
    eSetSampler,
    eSetConstantBuffer,
    eSetUnorderedAccessObject,

    eNum
  };
  static_assert(static_cast<vdl::uint>(ComputeCommandType::eNum) <= sizeof(vdl::uint8_t) * 8);
  struct ComputeState
  {
    vdl::ComputeShader ComputeShader;
    ShaderResources ShaderResources;
    Samplers Samplers;
    ConstantBuffers ConstantBuffers;
    UnorderedAccessObjects UnorderedAccessObjects;
  };
  struct ComputeReserveData
  {
    vdl::ComputeShader Shader;
    ShaderResources ShaderResources;
    ConstantBuffers ConstantBuffers;
    UnorderedAccessObjects UnorderedAccessObjects;
    std::vector<DescriptorHeap> ShaderResourceHeaps;
    std::vector<DescriptorHeap> SamplerHeaps;
    std::vector<DescriptorHeap> ConstantBufferHeaps;
    std::vector<DescriptorHeap> UnorderedAccessHeaps;

    std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> PipelineStates;
  public:
    ComputeReserveData() = default;

    void Clear()
    {
      ShaderResources.clear();
      ConstantBuffers.clear();
      UnorderedAccessObjects.clear();

      ShaderResourceHeaps.clear();
      SamplerHeaps.clear();
      ConstantBufferHeaps.clear();
      UnorderedAccessHeaps.clear();

      PipelineStates.clear();
    }
  };
private:
  ID3D12Device5* pD3D12Device_;
  IDXGISwapChain* pDXGISwapChain_;
private:
  CDevice* pDevice_;
  CSwapChain* pSwapChain_;
  ITextureManager* pTextureManager_;
  IBufferManager* pBufferManager_;
  IShaderManager* pShaderManager_;
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
private:
  ID3D12CommandQueue* pGraphicsCommandQueue_;
  Microsoft::WRL::ComPtr<ID3D12RootSignature> pGraphicsRootSignature_;
  std::array<CommandList, Constants::kGraphicsCommandBufferNum> GraphicsCommandLists_;
  vdl::uint GraphicsCommandBufferIndex_ = 0;
  StateChangeFlags<GraphicsCommandType, vdl::uint32_t> GraphicsStateChangeFlags_;
  vdl::uint GraphicsRenderTargetCount_;
  GraphicsState CurrentGraphicsState_;
  std::array<Fence, Constants::kGraphicsCommandBufferNum> GraphicsFences_;
  std::array<GraphicsReserveData, Constants::kGraphicsCommandBufferNum> GraphicsReserveDatas_;
private:
  ID3D12CommandQueue* pComputeCommandQueue_;
  Microsoft::WRL::ComPtr<ID3D12RootSignature> pComputeRootSignature_;
  std::array<CommandList, Constants::kGraphicsCommandBufferNum> ComputeCommandLists_;
  vdl::uint ComputeCommandBufferIndex_ = 0;
  StateChangeFlags<ComputeCommandType, vdl::uint8_t> ComputeStateChangeFlags_;
  ComputeState CurrentComputeState_;
  std::array<Fence, Constants::kComputeCommandBufferNum> ComputeFences_;
  std::array<ComputeReserveData, Constants::kComputeCommandBufferNum> ComputeReserveDatas_;
private:
  GraphicsReserveData& GetCurrentGraphicsReserveData() { return GraphicsReserveDatas_[GraphicsCommandBufferIndex_]; }
  CommandList& GetCurrentGraphicsCommandList() { return GraphicsCommandLists_[GraphicsCommandBufferIndex_]; }
  Fence& GetCurrentGraphicsFence() { return GraphicsFences_[GraphicsCommandBufferIndex_]; }
  ComputeReserveData& GetCurrentComputeReserveData() { return ComputeReserveDatas_[ComputeCommandBufferIndex_]; }
  CommandList& GetCurrentComputeCommandList() { return ComputeCommandLists_[ComputeCommandBufferIndex_]; }
  Fence& GetCurrentComputeFence() { return ComputeFences_[ComputeCommandBufferIndex_]; }
private:
  void PreprocessingDraw();
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
  void Present();
public:
  CDeviceContext() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

  void Initialize()override;

  void SetVertexBuffer(const VertexBuffer& _VertexBuffer)override;

  void SetInstanceBuffer(const InstanceBuffer& _InstanceBuffer)override;

  void SetIndexBuffer(const IndexBuffer& _IndexBuffer)override;

  void SetInputLayout(vdl::InputLayoutType _InputLayout)override;

  void SetTopology(vdl::TopologyType _Topology)override;

  void SetScissor(const vdl::Scissor& _Scissor)override;

  void SetViewport(const vdl::Viewport& _Viewport)override;

  void SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStencilTexture)override;

  void SetBlendState(const vdl::BlendState& _BlendState)override;

  void SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState)override;

  void SetRasterizerState(const vdl::RasterizerState& _RasterizerState)override;

  void VSSetShader(const vdl::VertexShader& _VertexShader)override;

  void VSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])override;

  void VSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void VSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void HSSetShader(const vdl::HullShader& _HullShader)override;

  void HSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])override;

  void HSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void HSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void DSSetShader(const vdl::DomainShader& _DomainShader)override;

  void DSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])override;

  void DSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void DSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void GSSetShader(const vdl::GeometryShader& _GeometryShader)override;

  void GSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])override;

  void GSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void GSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void PSSetShader(const vdl::PixelShader& _PixelShader)override;

  void PSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])override;

  void PSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void PSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void CSSetShader(const vdl::ComputeShader& _ComputeShader)override;

  void CSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])override;

  void CSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void CSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void CSSetUnorderedAccessObjects(vdl::uint _StartSlot, vdl::uint _UnorderedAccessObjectNum, const vdl::UnorderedAccessObject _UnorderedAccessObjects[])override;

  void ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::Color4F& _ClearColor)override;

  void ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)override;

  void ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::Color4F& _ClearColor)override;

  void Draw(vdl::uint _VertexCount, vdl::uint _InstanceCount, vdl::uint _FirstVertex, vdl::uint _FirstInstance)override;

  void DrawIndexed(vdl::uint _IndexCount, vdl::uint _InstanceCount, vdl::uint _FirstIndex, vdl::uint _VertexOffset, vdl::uint _FirstInstance)override;

  void Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)override;

  void Flush()override;
};
