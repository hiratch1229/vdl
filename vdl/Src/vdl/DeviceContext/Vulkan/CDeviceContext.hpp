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

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include <vdl/Shader/Vulkan/CShader.hpp>
#include <vdl/StateChangeFlags/StateChangeFlags.hpp>

#include <unordered_map>

class IWindow;
class CSwapChain;
class ITextureManager;
class IBufferManager;
class IShaderManager;

class CDeviceContext : public IDeviceContext
{
  static constexpr vdl::uint kComputeDescriptorLayoutNum = kComputeDescriptorTypeNum;
  static constexpr vdl::uint kGraphicsCommandBufferNum = 5;
  static constexpr vdl::uint kGraphicsDescriptorPoolMaxSet = kGraphicsDescriptorLayoutNum * kGraphicsCommandBufferNum * 10;
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
  static constexpr vdl::uint kGraphicsCommandTypeNum = static_cast<vdl::uint>(GraphicsCommandType::eNum);
  static_assert(kGraphicsCommandTypeNum <= sizeof(vdl::uint32_t) * 8);
  static constexpr GraphicsCommandType kGraphicsPipelineStateCommands[] = {
    GraphicsCommandType::eSetInputLayout, GraphicsCommandType::eSetTopology, GraphicsCommandType::eSetBlendState, GraphicsCommandType::eSetDepthStencilState, GraphicsCommandType::eSetRasterizerState,
    GraphicsCommandType::eSetVertexShader, GraphicsCommandType::eSetHullShader, GraphicsCommandType::eSetDomainShader, GraphicsCommandType::eSetGeometryShader, GraphicsCommandType::eSetPixelShader };
private:
  using ShaderResources = std::vector<vdl::ShaderResource>;
  using Samplers = std::vector<vdl::Sampler>;
  using ConstantBuffers = std::vector<vdl::Detail::ConstantBufferData>;
private:
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
  enum class CommandBufferState
  {
    eIdle,
    eBegin,
    eBeginRenderPass,
  };
  struct GraphicsState
  {
    const IBuffer* pVertexBuffer;
    const IBuffer* pInstanceBuffer;
    const IBuffer* pIndexBuffer;
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
  struct SyncState
  {
    vk::UniqueFence Fence;
    vk::UniqueSemaphore Semaphore;
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
  vdl::uint GraphicsCommandBufferIndex_ = 0;
  CommandBufferState GraphicsCommandBufferState_ = CommandBufferState::eIdle;
  vk::UniqueCommandPool GraphicsCommandPool_;
  vk::UniqueDescriptorPool GraphicsDescriptorPool_;
  std::array<vk::UniqueCommandBuffer, kGraphicsCommandBufferNum> GraphicsCommandBuffers_;
  std::array<SyncState, kGraphicsCommandBufferNum> GraphicsSyncStates_;
  std::array<RenderPassData, kGraphicsCommandBufferNum> RenderPassDatas_;
  vdl::uint GraphicsColorAttachmentCount_ = 0;
  std::array<vdl::OutputManager, kGraphicsCommandBufferNum> CurrentOutputManagers_;
  std::array<std::vector<vk::UniquePipeline>, kGraphicsCommandBufferNum> GraphicsPipelines_;
  std::array<std::vector<vk::UniqueDescriptorSet>, kGraphicsCommandBufferNum> DescriptorSets_;

  StateChangeFlags<GraphicsCommandType, vdl::uint32_t> GraphicsStateChangeFlags_;
  GraphicsState CurrentGraphicsState_;
  
  std::array<std::vector<vdl::VertexShader>, kGraphicsCommandBufferNum> ReserveVertexShaders_;
  std::array<std::vector<vdl::HullShader>, kGraphicsCommandBufferNum> ReserveHullShaders_;
  std::array<std::vector<vdl::DomainShader>, kGraphicsCommandBufferNum> ReserveDomainShaders_;
  std::array<std::vector<vdl::GeometryShader>, kGraphicsCommandBufferNum> ReserveGeometryShaders_;
  std::array<std::vector<vdl::PixelShader>, kGraphicsCommandBufferNum> ReservePixelShaders_;
  std::array<std::vector<ShaderResources>, kGraphicsCommandBufferNum> ReserveGraphicsShaderResources_;
  std::array<std::vector<ConstantBuffers>, kGraphicsCommandBufferNum> ReserveGraphicsConstantBuffers_;
  std::array<std::unordered_map<vdl::ID, vdl::Texture>, kGraphicsCommandBufferNum> ClearTextures_;
private:
  vk::UniquePipelineCache ComputePipelineCache_;
  std::array<vk::UniqueDescriptorSetLayout, kComputeDescriptorLayoutNum> ComputeDescriptorLayouts_;
  vk::UniqueCommandPool ComputeCommandPool_;
  vk::UniqueCommandBuffer ComputeCommandBuffer_;
private:
  const vk::CommandBuffer& GetCurrentGraphicsCommandBuffer()const { return GraphicsCommandBuffers_[GraphicsCommandBufferIndex_].get(); }
  const vk::Semaphore& GetCurrentGraphicsSemaphore()const { return GraphicsSyncStates_[GraphicsCommandBufferIndex_].Semaphore.get(); }
  const vk::Fence& GetCurrentGraphicsFence()const { return GraphicsSyncStates_[GraphicsCommandBufferIndex_].Fence.get(); }
private:
  void BeginGraphicsCommandBuffer();
  void PreprocessingGraphicsCommandBufferDraw();
  void WaitFence();
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

  void SetVertexBuffer(const IBuffer* _pVertexBuffer)override;

  void SetInstanceBuffer(const IBuffer* _pInstanceBuffer)override;

  void SetIndexBuffer(const IBuffer* _pIndexBuffer)override;

  void SetInputLayout(vdl::InputLayoutType _InputLayout)override;

  void SetTopology(vdl::TopologyType _Topology)override;

  void SetScissor(const vdl::Scissor& _Scissor)override;

  void SetViewport(const vdl::Viewport& _Viewport)override;

  void SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStenilTexture)override;

  void SetBlendState(const vdl::BlendState& _BlendState)override;

  void SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState)override;

  void SetRasterizerState(const vdl::RasterizerState& _RasterizerState)override;

  void VSSetShader(const vdl::VertexShader& _VertexShader)override;

  void VSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResource[])override;

  void VSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void VSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void HSSetShader(const vdl::HullShader& _HullShader)override;

  void HSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResource[])override;

  void HSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void HSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void DSSetShader(const vdl::DomainShader& _DomainShader)override;

  void DSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResource[])override;

  void DSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void DSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void GSSetShader(const vdl::GeometryShader& _GeometryShader)override;

  void GSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResource[])override;

  void GSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void GSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void PSSetShader(const vdl::PixelShader& _PixelShader)override;

  void PSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResource[])override;

  void PSSetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void PSSetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void CSSetShader(const vdl::ComputeShader& _ComputeShader)override;

  void CSSetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResource[])override;

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
