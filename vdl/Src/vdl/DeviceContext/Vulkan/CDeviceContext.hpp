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

#include <vdl/Buffer/Buffer.hpp>

#include <vdl/Shader/Vulkan/CShader.hpp>
#include <vdl/StateChangeFlags/StateChangeFlags.hpp>

#include <vdl/pch/Vulkan/pch.hpp>

#include <unordered_map>

class IWindow;
class CSwapChain;
class ITextureManager;
class IBufferManager;
class IShaderManager;

class CDeviceContext : public IDeviceContext
{
  static constexpr vdl::uint kGraphicsCommandBufferNum = 5;
  static constexpr vdl::uint kGraphicsDescriptorPoolMaxSet = kGraphicsDescriptorLayoutNum * kGraphicsCommandBufferNum * 10;
  static constexpr vdl::uint kComputeCommandBufferNum = 3;
  static constexpr vdl::uint kComputeDescriptorPoolMaxSet = kComputeDescriptorTypeNum * kComputeCommandBufferNum;
private:
  using ShaderResources = std::vector<vdl::ShaderResource>;
  using Samplers = std::vector<vdl::Sampler>;
  using ConstantBuffers = std::vector<vdl::Detail::ConstantBufferData>;
  using UnorderedAccessObjects = std::vector<vdl::UnorderedAccessObject>;
private:
  enum class CommandBufferState
  {
    eIdle,
    eBegin,
    eBeginRenderPass,
  };
  struct SyncState
  {
    vk::UniqueFence Fence;
    vk::UniqueSemaphore Semaphore;
  };
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
  static_assert(static_cast<vdl::uint>(GraphicsCommandType::eNum) <= sizeof(vdl::uint32_t) * 8);
  static constexpr GraphicsCommandType kGraphicsPipelineStateCommands[] = {
    GraphicsCommandType::eSetInputLayout, GraphicsCommandType::eSetTopology, GraphicsCommandType::eSetBlendState, GraphicsCommandType::eSetDepthStencilState, GraphicsCommandType::eSetRasterizerState,
    GraphicsCommandType::eSetVertexShader, GraphicsCommandType::eSetHullShader, GraphicsCommandType::eSetDomainShader, GraphicsCommandType::eSetGeometryShader, GraphicsCommandType::eSetPixelShader };
  struct InputLayout
  {
    std::vector<vk::VertexInputBindingDescription> VertexInputBindingDescriptions;
    std::vector<vk::VertexInputAttributeDescription> VertexInputAttributeDescriptions;
  };
  struct RenderPassData
  {
    vk::UniqueRenderPass RenderPass;
    vk::UniqueFramebuffer Framebuffer;
  };
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

    std::unordered_map<vdl::ID, vdl::Texture> ClearTextures;

    std::vector<RenderPassData> RenderPassDatas;
    std::vector<vk::UniquePipeline> Pipelines;
    std::vector<vk::UniqueDescriptorSet> DescriptorSets;
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

      ClearTextures.clear();

      RenderPassDatas.clear();
      Pipelines.clear();
      DescriptorSets.clear();
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

    vk::UniquePipeline Pipeline;
    std::vector<vk::UniqueDescriptorSet> DescriptorSets;
  public:
    ComputeReserveData() = default;

    void Clear()
    {
      ShaderResources.clear();
      ConstantBuffers.clear();
      UnorderedAccessObjects.clear();
      DescriptorSets.clear();
    }
  };
private:
  vk::Device VkDevice_;
  vk::Queue GraphicsQueue_;
  vk::Queue ComputeQueue_;
private:
  IWindow* pWindow_;
  CSwapChain* pSwapChain_;
  ITextureManager* pTextureManager_;
  IBufferManager* pBufferManager_;
  IShaderManager* pShaderManager_;
private:
  std::unordered_map<vdl::InputLayoutType, InputLayout> InputLayouts_;
  std::unordered_map<vdl::RasterizerState, vk::PipelineRasterizationStateCreateInfo> RasterizerStates_;
  std::unordered_map<bool, vk::PipelineMultisampleStateCreateInfo> MultisampleStates_;
  std::unordered_map<vdl::DepthStencilState, vk::PipelineDepthStencilStateCreateInfo> DepthStencilStates_;
  std::unordered_map<vdl::RenderTextureBlendState, vk::PipelineColorBlendAttachmentState> RenderTextureBlendStates_;
  std::unordered_map<vdl::Sampler, vk::UniqueSampler> Samplers_;
private:
  vk::UniquePipelineLayout PipelineLayout_;
  vk::Semaphore LastSemaphore_;
private:
  vk::UniquePipelineCache GraphicsPipelineCache_;
  std::array<vk::UniqueDescriptorSetLayout, kGraphicsDescriptorLayoutNum> GraphicsDescriptorLayouts_;
  vk::UniqueCommandPool GraphicsCommandPool_;
  vk::UniqueDescriptorPool GraphicsDescriptorPool_;
  vdl::uint GraphicsCommandBufferIndex_ = 0;
  CommandBufferState GraphicsCommandBufferState_ = CommandBufferState::eIdle;
  std::array<vk::UniqueCommandBuffer, kGraphicsCommandBufferNum> GraphicsCommandBuffers_;
  std::array<SyncState, kGraphicsCommandBufferNum> GraphicsSyncStates_;
  vdl::uint GraphicsColorAttachmentCount_ = 0;
  StateChangeFlags<GraphicsCommandType, vdl::uint32_t> GraphicsStateChangeFlags_;
  GraphicsState CurrentGraphicsState_;
  std::array<GraphicsReserveData, kGraphicsCommandBufferNum> GraphicsReserveDatas_;
private:
  vk::UniquePipelineCache ComputePipelineCache_;
  std::array<vk::UniqueDescriptorSetLayout, kComputeDescriptorTypeNum> ComputeDescriptorLayouts_;
  vk::UniqueCommandPool ComputeCommandPool_;
  vk::UniqueDescriptorPool ComputeDescriptorPool_;
  vdl::uint ComputeCommandBufferIndex_ = 0;
  std::array<vk::UniqueCommandBuffer, kComputeCommandBufferNum> ComputeCommandBuffers_;
  std::array<SyncState, kComputeCommandBufferNum> ComputeSyncStates_;
  StateChangeFlags<ComputeCommandType, vdl::uint8_t> ComputeStateChangeFlags_;
  ComputeState CurrentComputeState_;
  vk::Pipeline LastComputePipeline_;
  std::array<ComputeReserveData, kComputeCommandBufferNum> ComputeReserveDatas_;
private:
  const vk::CommandBuffer& GetCurrentGraphicsCommandBuffer()const { return GraphicsCommandBuffers_[GraphicsCommandBufferIndex_].get(); }
  const vk::Semaphore& GetCurrentGraphicsSemaphore()const { return GraphicsSyncStates_[GraphicsCommandBufferIndex_].Semaphore.get(); }
  const vk::Fence& GetCurrentGraphicsFence()const { return GraphicsSyncStates_[GraphicsCommandBufferIndex_].Fence.get(); }
  const vk::CommandBuffer& GetCurrentComputeCommandBuffer()const { return ComputeCommandBuffers_[ComputeCommandBufferIndex_].get(); }
  const vk::Semaphore& GetCurrentComputeSemaphore()const { return ComputeSyncStates_[ComputeCommandBufferIndex_].Semaphore.get(); }
  const vk::Fence& GetCurrentComputeFence()const { return ComputeSyncStates_[ComputeCommandBufferIndex_].Fence.get(); }
private:
  void BeginGraphicsCommandBuffer();
  void BeginRenderPassGraphicsCommandBuffer();
  void PreprocessingGraphicsCommandBufferDraw();
  void WaitFence(const vk::Fence& _Fence);
  const vk::PipelineRasterizationStateCreateInfo& GetPipelineRasterizationStateInfo(const vdl::RasterizerState& _RasterizerState);
  const vk::PipelineDepthStencilStateCreateInfo& GetPipelineDepthStencilStateInfo(const vdl::DepthStencilState& _DepthStencilState);
  const vk::PipelineColorBlendAttachmentState& GetPipelineColorBlendAttachmentState(const vdl::RenderTextureBlendState& _RenderTextureBlendState);
  const vk::PipelineMultisampleStateCreateInfo& GetMultisampleStateInfo(bool _AlphaToCoverageEnable);
  const vk::Sampler& GetSampler(const vdl::Sampler& _Sampler);
public:
  void Present();
public:
  CDeviceContext() = default;

  void Initialize()override;

  void SetVertexBuffer(const VertexBuffer& _VertexBuffer)override;

  void SetInstanceBuffer(const InstanceBuffer& _InstanceBuffer)override;

  void SetIndexBuffer(const IndexBuffer& _IndexBuffer)override;

  void SetInputLayout(vdl::InputLayoutType _InputLayout)override;

  void SetTopology(vdl::TopologyType _Topology)override;

  void SetScissor(const vdl::Scissor& _Scissor)override;

  void SetViewport(const vdl::Viewport& _Viewport)override;

  void SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStenilTexture)override;

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

  void ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)override;

  void ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)override;

  void ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::ColorF& _ClearColor)override;

  void Draw(vdl::uint _VertexCount, vdl::uint _InstanceCount, vdl::uint _FirstVertex, vdl::uint _FirstInstance)override;

  void DrawIndexed(vdl::uint _IndexCount, vdl::uint _InstanceCount, vdl::uint _FirstIndex, vdl::uint _VertexOffset, vdl::uint _FirstInstance)override;

  void Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)override;

  void Flush()override;
};
