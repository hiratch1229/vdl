#pragma once
#include <vdl/Scissor.hpp>
#include <vdl/Viewport.hpp>
#include <vdl/GraphicsState.hpp>
#include <vdl/Shader.hpp>
#include <vdl/Texture.hpp>
#include <vdl/Sampler.hpp>
#include <vdl/ConstantBuffer.hpp>

#include <vdl/Model.hpp>

#include <vdl/Shader/IShader.hpp>
#include <vdl/Constants/Constants.hpp>

#include <array>
#include <vector>
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>

class IDevice;
class IDeviceContext;
struct IBuffer;
class IBufferManager;
class IModelManager;

enum class RendererCommandType
{
  eDraw = 0,
  eSetTopology,
  eSetScissor,
  eSetViewport,
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
static_assert(static_cast<vdl::uint32_t>(RendererCommandType::eNum) <= sizeof(vdl::uint32_t) * 8);

using RendererCommandPair = std::pair<RendererCommandType, vdl::uint>;
using RendererCommands = std::vector<RendererCommandPair>;

class StateChangeFlags
{
  vdl::uint32_t Flags_;
public:
  StateChangeFlags() = default;

  void Clear() { Flags_ = 0; }

  [[nodiscard]] bool isEmpty()const { return Flags_ == 0; }

  [[nodiscard]] bool Has(RendererCommandType _Command)const { return Flags_ & (1 << static_cast<vdl::uint>(_Command)); }

  void Set(RendererCommandType _Command) { Flags_ |= (1 << static_cast<vdl::uint>(_Command)); }

  void Cancel(RendererCommandType _Command) { Flags_ &= ~(1 << static_cast<vdl::uint>(_Command)); }
};

class Empty
{

};

template<class DisplayObject, class InstanceData>
class RendererCommandList
{
  static constexpr vdl::uint kShaderTypes = static_cast<vdl::uint>(ShaderType::eGraphicsNum);
  static_assert(std::is_same<DisplayObject, Empty>::value || std::is_same<DisplayObject, vdl::Texture>::value
    || std::is_same<DisplayObject, vdl::StaticMesh>::value || std::is_same<DisplayObject, vdl::SkinnedMesh>::value);
  static constexpr vdl::uint kMaxBatchNum = (std::is_same<DisplayObject, Empty>::value ? 1 : std::is_same<DisplayObject, vdl::Texture>::value ? Constants::kMaxTextureBatchNum
    : std::is_same<DisplayObject, vdl::StaticMesh>::value ? Constants::kMaxStaticMeshBatchNum : Constants::kMaxSkinnedMeshBatchNum);
private:
  using Textures = std::vector<vdl::Texture>;
  using ShaderResources = std::vector<vdl::ShaderResource>;
  using Samplers = std::vector<vdl::Sampler>;
  using ConstantBuffers = std::vector<vdl::Detail::ConstantBufferData>;
private:
  IDevice* pDevice_;
  IModelManager* pModelManager_;
  IBufferManager* pBufferManager_;
private:
  RendererCommands RendererCommands_;

  StateChangeFlags StateChangeFlags_;

  std::thread Thread_;

  std::vector<vdl::ID> DisplayObjectIDs_;
  std::vector<InstanceData> Instances_;

  std::vector<vdl::Scissor> Scissors_;
  std::vector<vdl::TopologyType> Topologys_;
  std::vector<vdl::Viewport> Viewports_;
  std::vector<vdl::BlendState> BlendStates_;
  std::vector<vdl::DepthStencilState> DepthStencilStates_;
  std::vector<vdl::RasterizerState> RasterizerStates_;
  std::vector<vdl::VertexShader> VertexShaders_;
  std::vector<vdl::HullShader> HullShaders_;
  std::vector<vdl::DomainShader> DomainShaders_;
  std::vector<vdl::GeometryShader> GeometryShaders_;
  std::vector<vdl::PixelShader> PixelShaders_;
  std::array<std::vector<ShaderResources>, kShaderTypes> ShaderResources_;
  std::array<std::vector<Samplers>, kShaderTypes> Samplers_;
  std::array<ConstantBuffers, kShaderTypes> LastConstantBuffers_;
  std::array<std::vector<ConstantBuffers>, kShaderTypes> ConstantBuffers_;

  vdl::TopologyType CurrentTopology_;
  vdl::Scissor CurrentScissor_;
  vdl::Viewport CurrentViewport_;
  vdl::BlendState CurrentBlendState_;
  vdl::DepthStencilState CurrentDepthStencilState_;
  vdl::RasterizerState CurrentRasterizerState_;
  vdl::VertexShader CurrentVertexShader_;
  vdl::HullShader CurrentHullShader_;
  vdl::DomainShader CurrentDomainShader_;
  vdl::GeometryShader CurrentGeometryShader_;
  vdl::PixelShader CurrentPixelShader_;
  std::array<ShaderResources, kShaderTypes> CurrentShaderResources_;
  std::array<Samplers, kShaderTypes> CurrentSamplers_;
  std::array<ConstantBuffers, kShaderTypes> CurrentConstantBuffers_;

  std::unordered_map<vdl::ID, DisplayObject> ReservedDisplayObjects_;
public:
  RendererCommandList() = default;

  void Initialize(vdl::TopologyType _Topology, const vdl::BlendState& _BlendState, const vdl::DepthStencilState& _DepthStencilState, const vdl::RasterizerState& _RasterizerState,
    const vdl::Sampler& _Sampler, const vdl::VertexShader& _VertexShader, const vdl::PixelShader& _PixelShader);

    void Reset();

  void Adjust();

  void Flush(IDeviceContext* _pDeviceContext, IBuffer* _pInstanceBuffer);

  [[nodiscard]] bool HasDrawCommand()const { return !Instances_.empty(); }

  void PushDrawData(const DisplayObject& _DisplayObject, InstanceData&& _InstanceData);
  [[nodiscard]] const InstanceData& GetInstanceData(vdl::uint _Index)const { return Instances_[_Index]; }

  void PushTopology(const vdl::TopologyType& _Topology);
  [[nodiscard]] const vdl::TopologyType& GetCurrentTopology()const { return CurrentTopology_; }

  void PushScissor(const vdl::Scissor& _Scissor);
  [[nodiscard]] const vdl::Scissor& GetCurrentScissor()const { return CurrentScissor_; }

  void PushViewport(const vdl::Viewport& _Viewport);
  [[nodiscard]] const vdl::Viewport& GetCurrentViewport()const { return CurrentViewport_; }

  void PushBlendState(const vdl::BlendState& _BlendState);
  [[nodiscard]] const vdl::BlendState& GetCurrentBlendState()const { return CurrentBlendState_; }

  void PushDepthStencilState(const vdl::DepthStencilState& _DepthStencilState);
  [[nodiscard]] const vdl::DepthStencilState& GetCurrentDepthStencilState()const { return CurrentDepthStencilState_; }

  void PushRasterizerState(const vdl::RasterizerState& _RasterizerState);
  [[nodiscard]] const vdl::RasterizerState& GetCurrentRasterizerState()const { return CurrentRasterizerState_; }

  void PushVertexShader(const vdl::VertexShader& _VertexShader);
  [[nodiscard]] const vdl::VertexShader& GetCurrentVertexShader()const { return CurrentVertexShader_; }

  void PushHullShader(const vdl::HullShader& _HullShader);
  [[nodiscard]] const vdl::HullShader& GetCurrentHullShader()const { return CurrentHullShader_; }

  void PushDomainShader(const vdl::DomainShader& _DomainShader);
  [[nodiscard]] const vdl::DomainShader& GetCurrentDomainShader()const { return CurrentDomainShader_; }

  void PushGeometryShader(const vdl::GeometryShader& _GeometryShader);
  [[nodiscard]] const vdl::GeometryShader& GetCurrentGeometryShader()const { return CurrentGeometryShader_; }

  void PushPixelShader(const vdl::PixelShader& _PixelShader);
  [[nodiscard]] const vdl::PixelShader& GetCurrentPixelShader()const { return CurrentPixelShader_; }

  void PushVertexStageShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]);
  [[nodiscard]] const vdl::ShaderResource& GetCurrentVertexStageShaderResource(vdl::uint _Slot)const { return CurrentShaderResources_[static_cast<vdl::uint>(ShaderType::eVertexShader)][_Slot]; }

  void PushHullStageShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]);
  [[nodiscard]] const vdl::ShaderResource& GetCurrentHullStageShaderResource(vdl::uint _Slot)const { return CurrentShaderResources_[static_cast<vdl::uint>(ShaderType::eHullShader)][_Slot]; }

  void PushDomainStageShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]);
  [[nodiscard]] const vdl::ShaderResource& GetCurrentDomainStageShaderResource(vdl::uint _Slot)const { return CurrentShaderResources_[static_cast<vdl::uint>(ShaderType::eDomainShader)][_Slot]; }

  void PushGeometryStageShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]);
  [[nodiscard]] const vdl::ShaderResource& GetCurrentGeometryStageShaderResource(vdl::uint _Slot)const { return CurrentShaderResources_[static_cast<vdl::uint>(ShaderType::eGeometryShader)][_Slot]; }

  void PushPixelStageShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]);
  [[nodiscard]] const vdl::ShaderResource& GetCurrentPixelStageShaderResource(vdl::uint _Slot)const { return CurrentShaderResources_[static_cast<vdl::uint>(ShaderType::ePixelShader)][_Slot]; }

  //void PushVertexStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot);
  void PushVertexStageSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]);
  [[nodiscard]] const vdl::Sampler& GetCurrentVertexStageSampler(vdl::uint _Slot)const { return CurrentSamplers_[static_cast<vdl::uint>(ShaderType::eVertexShader)][_Slot]; }

  //void PushHullStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot);
  void PushHullStageSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]);
  [[nodiscard]] const vdl::Sampler& GetCurrentHullStageSampler(vdl::uint _Slot)const { return CurrentSamplers_[static_cast<vdl::uint>(ShaderType::eHullShader)][_Slot]; }

  //void PushDomainStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot);
  void PushDomainStageSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]);
  [[nodiscard]] const vdl::Sampler& GetCurrentDomainStageSampler(vdl::uint _Slot)const { return CurrentSamplers_[static_cast<vdl::uint>(ShaderType::eDomainShader)][_Slot]; }

  //void PushGeometryStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot);
  void PushGeometryStageSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]);
  [[nodiscard]] const vdl::Sampler& GetCurrentGeometryStageSampler(vdl::uint _Slot)const { return CurrentSamplers_[static_cast<vdl::uint>(ShaderType::eGeometryShader)][_Slot]; }

  //void PushPixelStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot);
  void PushPixelStageSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]);
  [[nodiscard]] const vdl::Sampler& GetCurrentPixelStageSampler(vdl::uint _Slot)const { return CurrentSamplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][_Slot]; }

  //void PushVertexStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot);
  void PushVertexStageConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]);
  [[nodiscard]] const vdl::Detail::ConstantBufferData& GetCurrentVertexStageConstantBuffer(vdl::uint _Slot)const { return CurrentConstantBuffers_[static_cast<vdl::uint>(ShaderType::eVertexShader)][_Slot]; }

  //void PushHullStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot);
  void PushHullStageConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]);
  [[nodiscard]] const vdl::Detail::ConstantBufferData& GetCurrentHullStageConstantBuffer(vdl::uint _Slot)const { return CurrentConstantBuffers_[static_cast<vdl::uint>(ShaderType::eHullShader)][_Slot]; }

  //void PushDomainStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot);
  void PushDomainStageConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]);
  [[nodiscard]] const vdl::Detail::ConstantBufferData& GetCurrentDomainStageConstantBuffer(vdl::uint _Slot)const { return CurrentConstantBuffers_[static_cast<vdl::uint>(ShaderType::eDomainShader)][_Slot]; }

  //void PushGeometryStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot);
  void PushGeometryStageConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]);
  [[nodiscard]] const vdl::Detail::ConstantBufferData& GetCurrentGeometryStageConstantBuffer(vdl::uint _Slot)const { return CurrentConstantBuffers_[static_cast<vdl::uint>(ShaderType::eGeometryShader)][_Slot]; }

  //void PushPixelStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot);
  void PushPixelStageConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]);
  [[nodiscard]] const vdl::Detail::ConstantBufferData& GetCurrentPixelStageConstantBuffer(vdl::uint _Slot)const { return CurrentConstantBuffers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][_Slot]; }
};

#include "RendererCommand.inl"
