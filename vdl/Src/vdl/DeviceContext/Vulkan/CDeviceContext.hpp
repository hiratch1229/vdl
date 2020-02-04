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

#include <vdl/pch/Vulkan/pch.hpp>

#include <vdl/Texture/Vulkan/CTexture.hpp>
#include <vdl/Shader/Vulkan/CShader.hpp>

#include <array>
#include <unordered_map>

class IDevice;
class CSwapChain;
class ITextureManager;
class IBufferManager;
class IShaderManager;

class CDeviceContext : public IDeviceContext
{
  static constexpr vdl::uint kDescriptorMultipleNum = 100;
  static constexpr vdl::uint kGraphicsDescriptorPoolMaxSet = kGraphicsDescriptorLayoutNum * Constants::kGraphicsCommandBufferNum * kDescriptorMultipleNum;
  static constexpr vdl::uint kComputeDescriptorPoolMaxSet = kComputeDescriptorTypeNum * Constants::kComputeCommandBufferNum * kDescriptorMultipleNum;
private:
  using Texture = vdl::Variant<vdl::Texture, vdl::DepthStencilTexture>;
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
  struct InputLayout
  {
    std::vector<vk::VertexInputBindingDescription> VertexInputBindingDescriptions;
    std::vector<vk::VertexInputAttributeDescription> VertexInputAttributeDescriptions;
  };
  struct Topology
  {
    vk::PipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateInfo;
    vk::PipelineTessellationStateCreateInfo PipelineTessellationStateInfo;
  };
  struct Sampler
  {
    vk::UniqueSampler VkSampler;
    vk::DescriptorImageInfo Descriptor;
  };
  struct RenderPassData
  {
    vk::UniqueRenderPass RenderPass;
    vk::UniqueFramebuffer Framebuffer;
  };
  struct GraphicsReserveData
  {
    std::vector<vdl::OutputManager> OutputManagers;

    std::unordered_map<vdl::ID, Texture> ClearTextures;

    std::vector<RenderPassData> RenderPassDatas;
    std::vector<vk::UniquePipeline> Pipelines;
    std::vector<vk::UniqueDescriptorSet> DescriptorSets;
  public:
    GraphicsReserveData() = default;

    void Clear()
    {
      OutputManagers.clear();

      ClearTextures.clear();

      RenderPassDatas.clear();
      Pipelines.clear();
      DescriptorSets.clear();
    }
  };
private:
  struct ComputeReserveData
  {
    std::vector<vk::UniquePipeline> Pipelines;
    std::vector<vk::UniqueDescriptorSet> TextureDescriptorSets;
    std::vector<vk::UniqueDescriptorSet> BufferDescriptorSets;
    std::vector<vk::UniqueDescriptorSet> SamplerDescriptorSets;
    std::vector<vk::UniqueDescriptorSet> ConstantBufferDescriptorSets;
    std::vector<vk::UniqueDescriptorSet> UnorderedAccessTextureDescriptorSets;
    std::vector<vk::UniqueDescriptorSet> UnorderedAccessBufferDescriptorSets;
  public:
    ComputeReserveData() = default;

    void Clear()
    {
      Pipelines.clear();
      TextureDescriptorSets.clear();
      BufferDescriptorSets.clear();
      SamplerDescriptorSets.clear();
      ConstantBufferDescriptorSets.clear();
      UnorderedAccessTextureDescriptorSets.clear();
      UnorderedAccessBufferDescriptorSets.clear();
    }
  };
private:
  vk::Device VkDevice_;
  vk::Queue GraphicsQueue_;
  vk::Queue ComputeQueue_;
private:
  IDevice* pDevice_;
  CSwapChain* pSwapChain_;
  ITextureManager* pTextureManager_;
  IBufferManager* pBufferManager_;
  IShaderManager* pShaderManager_;
private:
  std::unordered_map<vdl::InputLayoutType, InputLayout> InputLayouts_;
  std::unordered_map<vdl::InputLayoutType, vk::PipelineVertexInputStateCreateInfo> PipelineVertexInputStateInfos_;
  std::unordered_map<vdl::TopologyType, Topology> Topologys_;
  std::array<vk::DynamicState, 2> DynamicStates_;
  vk::PipelineDynamicStateCreateInfo PipelineDynamicStateInfo_;
  vk::PipelineColorBlendStateCreateInfo PipelineColorBlendStateInfo_;
  vk::PipelineViewportStateCreateInfo PipelineViewportStateInfo_;
  std::array<vk::PipelineColorBlendAttachmentState, Constants::kMaxRenderTextureNum> PipelineColorBlendAttachmentStates_;
  std::unordered_map<vdl::RasterizerState, vk::PipelineRasterizationStateCreateInfo> RasterizerStates_;
  std::unordered_map<bool, vk::PipelineMultisampleStateCreateInfo> MultisampleStates_;
  std::unordered_map<vdl::DepthStencilState, vk::PipelineDepthStencilStateCreateInfo> DepthStencilStates_;
  std::unordered_map<vdl::RenderTextureBlendState, vk::PipelineColorBlendAttachmentState> RenderTextureBlendStates_;
  std::unordered_map<vdl::Sampler, Sampler> Samplers_;
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
  bool isChangeGraphicsPipelineState_ = true;
  vk::GraphicsPipelineCreateInfo GraphicsPipelineInfo_;
  std::array<vk::UniqueCommandBuffer, Constants::kGraphicsCommandBufferNum> GraphicsCommandBuffers_;
  std::array<SyncState, Constants::kGraphicsCommandBufferNum> GraphicsSyncStates_;
  //vdl::uint GraphicsColorAttachmentCount_ = 0;
  std::array<BaseGraphicsCommandList, Constants::kGraphicsCommandBufferNum> GraphicsCommandListDatas_;
  std::array<GraphicsReserveData, Constants::kGraphicsCommandBufferNum> GraphicsReserveDatas_;
private:
  vk::UniquePipelineCache ComputePipelineCache_;
  std::array<vk::UniqueDescriptorSetLayout, kComputeDescriptorTypeNum> ComputeDescriptorLayouts_;
  vk::UniqueCommandPool ComputeCommandPool_;
  vk::UniqueDescriptorPool ComputeDescriptorPool_;
  vdl::uint ComputeCommandBufferIndex_ = 0;
  std::array<vk::UniqueCommandBuffer, Constants::kComputeCommandBufferNum> ComputeCommandBuffers_;
  std::array<SyncState, Constants::kComputeCommandBufferNum> ComputeSyncStates_;
  std::array<ComputeCommandList, Constants::kComputeCommandBufferNum> ComputeCommandListDatas_;
  vk::Pipeline LastComputePipeline_;
  std::array<ComputeReserveData, Constants::kComputeCommandBufferNum> ComputeReserveDatas_;
private:
  const vk::CommandBuffer& GetCurrentGraphicsCommandBuffer()const { return GraphicsCommandBuffers_[GraphicsCommandBufferIndex_].get(); }
  const vk::Semaphore& GetCurrentGraphicsSemaphore()const { return GraphicsSyncStates_[GraphicsCommandBufferIndex_].Semaphore.get(); }
  const vk::Fence& GetCurrentGraphicsFence()const { return GraphicsSyncStates_[GraphicsCommandBufferIndex_].Fence.get(); }
  GraphicsReserveData& GetCurrentGraphicsReserveData() { return GraphicsReserveDatas_[GraphicsCommandBufferIndex_]; }
  const vk::CommandBuffer& GetCurrentComputeCommandBuffer()const { return ComputeCommandBuffers_[ComputeCommandBufferIndex_].get(); }
  const vk::Semaphore& GetCurrentComputeSemaphore()const { return ComputeSyncStates_[ComputeCommandBufferIndex_].Semaphore.get(); }
  const vk::Fence& GetCurrentComputeFence()const { return ComputeSyncStates_[ComputeCommandBufferIndex_].Fence.get(); }
  ComputeReserveData& GetCurrentComputeReserveData() { return ComputeReserveDatas_[ComputeCommandBufferIndex_]; }
private:
  void BeginGraphicsCommandBuffer();
  void BeginRenderPassGraphicsCommandBuffer();
  void WaitFence(const vk::Fence& _Fence);
  CRenderTexture* GetVkRenderTexture(const vdl::RenderTexture& _RenderTexture);
  const vk::PipelineRasterizationStateCreateInfo& GetPipelineRasterizationStateInfo(const vdl::RasterizerState& _RasterizerState);
  const vk::PipelineDepthStencilStateCreateInfo& GetPipelineDepthStencilStateInfo(const vdl::DepthStencilState& _DepthStencilState);
  const vk::PipelineColorBlendAttachmentState& GetPipelineColorBlendAttachmentState(const vdl::RenderTextureBlendState& _RenderTextureBlendState);
  const vk::PipelineMultisampleStateCreateInfo& GetMultisampleStateInfo(bool _AlphaToCoverageEnable);
  const vk::DescriptorImageInfo& GetTextureDescriptor(ITexture* _pTexture, const vk::CommandBuffer& _CommandBuffer);
  const vk::DescriptorImageInfo& GetSamplerDescriptor(const vdl::Sampler& _Sampler);
  const vk::DescriptorBufferInfo& GetConstantBufferDescriptor(const vdl::Detail::ConstantBufferData& _ConstantBuffer);
  const vk::DescriptorImageInfo& GetUnorderedAccessTextureDescriptor(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vk::CommandBuffer& _CommandBuffer);
  const vk::DescriptorBufferInfo& GetUnorderedAccessBufferDescriptor(const vdl::Detail::UnorderedAccessBufferData& _UnorderedAccessBuffer);
public:
  void Present();
public:
  CDeviceContext() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eVulkan; }

  void Initialize()override;

  void SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStenilTexture)override;

  void ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::Color4F& _ClearColor)override;

  void ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)override;

  void ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::Color4F& _ClearColor)override;

  void Flush()override;

  void Execute(const BaseGraphicsCommandList& _GraphicsCommandList)override;

  void Execute(const ComputeCommandList& _ComputeCommandList)override;
};
