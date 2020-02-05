#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Scissor.hpp>
#include <vdl/Viewport.hpp>
#include <vdl/GraphicsStates.hpp>
#include <vdl/Shader.hpp>
#include <vdl/Texture.hpp>
#include <vdl/Sampler.hpp>
#include <vdl/ConstantBuffer.hpp>
#include <vdl/UnorderedAccessBuffer.hpp>
#include <vdl/Variant.hpp>
#include <vdl/Flag.hpp>

#include <vdl/Buffer/Buffer.hpp>
#include <vdl/Shader/IShader.hpp>

#include <array>
#include <vector>
#include <unordered_map>
#include <any>
#include <functional>
#include <shared_mutex>
#include <condition_variable>

enum class GraphicsCommandFlag : vdl::uint32_t
{
  eDraw = 1u << 0,
  eDrawIndexed = 1u << 1,
  eSetVertexBuffer = 1u << 2,
  eSetInstanceBuffer = 1u << 3,
  eSetIndexBuffer = 1u << 4,
  eSetInputLayout = 1u << 5,
  eSetTopology = 1u << 6,
  eSetScissor = 1u << 7,
  eSetViewport = 1u << 8,
  eSetBlendState = 1u << 9,
  eSetDepthStencilState = 1u << 10,
  eSetRasterizerState = 1u << 11,
  eSetVertexShader = 1u << 12,
  eSetHullShader = 1u << 13,
  eSetDomainShader = 1u << 14,
  eSetGeometryShader = 1u << 15,
  eSetPixelShader = 1u << 16,
  eSetVertexStageShaderResource = 1u << 17,
  eSetHullStageShaderResource = 1u << 18,
  eSetDomainStageShaderResource = 1u << 19,
  eSetGeometryStageShaderResource = 1u << 20,
  eSetPixelStageShaderResource = 1u << 21,
  eSetVertexStageSampler = 1u << 22,
  eSetHullStageSampler = 1u << 23,
  eSetDomainStageSampler = 1u << 24,
  eSetGeometryStageSampler = 1u << 25,
  eSetPixelStageSampler = 1u << 26,
  eSetVertexStageConstantBuffer = 1u << 27,
  eSetHullStageConstantBuffer = 1u << 28,
  eSetDomainStageConstantBuffer = 1u << 29,
  eSetGeometryStageConstantBuffer = 1u << 30,
  eSetPixelStageConstantBuffer = 1u << 31,
};
using GraphicsCommandFlags = vdl::Flags<GraphicsCommandFlag, vdl::uint32_t>;

class IBufferManager;
class IModelManager;
class ThreadPool;

struct DrawData
{
  vdl::uint VertexCount;
  vdl::uint InstanceCount;
  vdl::uint FirstVertex;
  vdl::uint FirstInstance;
public:
  DrawData() = default;

  constexpr DrawData(vdl::uint _VertexCount, vdl::uint _InstanceCount, vdl::uint _FirstVertex, vdl::uint _FirstInstance)
    : VertexCount(_VertexCount), InstanceCount(_InstanceCount), FirstVertex(_FirstVertex), FirstInstance(_FirstInstance) {}
};
struct DrawIndexedData
{
  vdl::uint IndexCount;
  vdl::uint InstanceCount;
  vdl::uint FirstIndex;
  vdl::uint VertexOffset;
  vdl::uint FirstInstance;
public:
  DrawIndexedData() = default;

  constexpr DrawIndexedData(vdl::uint _IndexCount, vdl::uint _InstanceCount, vdl::uint _FirstIndex, vdl::uint _VertexOffset, vdl::uint _FirstInstance)
    : IndexCount(_IndexCount), InstanceCount(_InstanceCount), FirstIndex(_FirstIndex), VertexOffset(_VertexOffset), FirstInstance(_FirstInstance) {}
};

class BaseGraphicsCommandList
{
protected:
  using GraphicsCommandPair = std::pair<GraphicsCommandFlag, vdl::uint>;
  using GraphicsCommands = std::vector<GraphicsCommandPair>;
  using ShaderResources = std::vector<vdl::ShaderResource>;
  using Samplers = std::vector<vdl::Sampler>;
  using ConstantBuffers = std::vector<vdl::Detail::ConstantBufferData>;
  using InstanceDatas = std::vector<vdl::uint8_t>;
protected:
  static constexpr GraphicsCommandFlags kDrawFlag = GraphicsCommandFlag::eDraw;
  static constexpr GraphicsCommandFlags kDrawIndexedFlag = GraphicsCommandFlag::eDrawIndexed;
  static constexpr GraphicsCommandFlags kSetVertexBufferFlag = GraphicsCommandFlag::eSetVertexBuffer;
  static constexpr GraphicsCommandFlags kSetInstanceBufferFlag = GraphicsCommandFlag::eSetInstanceBuffer;
  static constexpr GraphicsCommandFlags kSetIndexBufferFlag = GraphicsCommandFlag::eSetIndexBuffer;
  static constexpr GraphicsCommandFlags kSetInputLayoutFlag = GraphicsCommandFlag::eSetInputLayout;
  static constexpr GraphicsCommandFlags kSetTopologyFlag = GraphicsCommandFlag::eSetTopology;
  static constexpr GraphicsCommandFlags kSetScissorFlag = GraphicsCommandFlag::eSetScissor;
  static constexpr GraphicsCommandFlags kSetViewportFlag = GraphicsCommandFlag::eSetViewport;
  static constexpr GraphicsCommandFlags kSetBlendStateFlag = GraphicsCommandFlag::eSetBlendState;
  static constexpr GraphicsCommandFlags kSetDepthStencilStateFlag = GraphicsCommandFlag::eSetDepthStencilState;
  static constexpr GraphicsCommandFlags kSetRasterizerStateFlag = GraphicsCommandFlag::eSetRasterizerState;
  static constexpr GraphicsCommandFlags kSetVertexShaderFlag = GraphicsCommandFlag::eSetVertexShader;
  static constexpr GraphicsCommandFlags kSetHullShaderFlag = GraphicsCommandFlag::eSetHullShader;
  static constexpr GraphicsCommandFlags kSetDomainShaderFlag = GraphicsCommandFlag::eSetDomainShader;
  static constexpr GraphicsCommandFlags kSetGeometryShaderFlag = GraphicsCommandFlag::eSetGeometryShader;
  static constexpr GraphicsCommandFlags kSetPixelShaderFlag = GraphicsCommandFlag::eSetPixelShader;
  static constexpr GraphicsCommandFlags kSetVertexStageShaderResourceFlag = GraphicsCommandFlag::eSetVertexStageShaderResource;
  static constexpr GraphicsCommandFlags kSetHullStageShaderResourceFlag = GraphicsCommandFlag::eSetHullStageShaderResource;
  static constexpr GraphicsCommandFlags kSetDomainStageShaderResourceFlag = GraphicsCommandFlag::eSetDomainStageShaderResource;
  static constexpr GraphicsCommandFlags kSetGeometryStageShaderResourceFlag = GraphicsCommandFlag::eSetGeometryStageShaderResource;
  static constexpr GraphicsCommandFlags kSetPixelStageShaderResourceFlag = GraphicsCommandFlag::eSetPixelStageShaderResource;
  static constexpr GraphicsCommandFlags kSetVertexStageSamplerFlag = GraphicsCommandFlag::eSetVertexStageSampler;
  static constexpr GraphicsCommandFlags kSetHullStageSamplerFlag = GraphicsCommandFlag::eSetHullStageSampler;
  static constexpr GraphicsCommandFlags kSetDomainStageSamplerFlag = GraphicsCommandFlag::eSetDomainStageSampler;
  static constexpr GraphicsCommandFlags kSetGeometryStageSamplerFlag = GraphicsCommandFlag::eSetGeometryStageSampler;
  static constexpr GraphicsCommandFlags kSetPixelStageSamplerFlag = GraphicsCommandFlag::eSetPixelStageSampler;
  static constexpr GraphicsCommandFlags kSetVertexStageConstantBufferFlag = GraphicsCommandFlag::eSetVertexStageConstantBuffer;
  static constexpr GraphicsCommandFlags kSetHullStageConstantBufferFlag = GraphicsCommandFlag::eSetHullStageConstantBuffer;
  static constexpr GraphicsCommandFlags kSetDomainStageConstantBufferFlag = GraphicsCommandFlag::eSetDomainStageConstantBuffer;
  static constexpr GraphicsCommandFlags kSetGeometryStageConstantBufferFlag = GraphicsCommandFlag::eSetGeometryStageConstantBuffer;
  static constexpr GraphicsCommandFlags kSetPixelStageConstantBufferFlag = GraphicsCommandFlag::eSetPixelStageConstantBuffer;
protected:
  IBufferManager* pBufferManager_;

protected:
  GraphicsCommands GraphicsCommands_;
  GraphicsCommandFlags GraphicsCommandFlags_;
  std::vector<vdl::ID> DisplayObjectIDs_;
  std::vector<InstanceDatas> Instances_;
  std::vector<DrawData> DrawDatas_;
  std::vector<DrawIndexedData> DrawIndexedDatas_;
  std::unordered_map<vdl::ID, std::any> ReservedDisplayObjects_;

  InstanceBuffer InstanceBuffer_;
  std::vector<VertexBuffer> VertexBuffers_;
  std::vector<IndexBuffer> IndexBuffers_;
  std::vector<vdl::InputLayoutType> InputLayouts_;
  std::vector<vdl::TopologyType> Topologys_;
  std::vector<vdl::Scissor> Scissors_;
  std::vector<vdl::Viewport> Viewports_;
  std::vector<vdl::BlendState> BlendStates_;
  std::vector<vdl::DepthStencilState> DepthStencilStates_;
  std::vector<vdl::RasterizerState> RasterizerStates_;
  std::vector<vdl::VertexShader> VertexShaders_;
  std::vector<vdl::HullShader> HullShaders_;
  std::vector<vdl::DomainShader> DomainShaders_;
  std::vector<vdl::GeometryShader> GeometryShaders_;
  std::vector<vdl::PixelShader> PixelShaders_;
  std::array<std::vector<ShaderResources>, kGraphicsShaderStageNum> ShaderResources_;
  std::array<std::vector<Samplers>, kGraphicsShaderStageNum> Samplers_;
  std::array<ConstantBuffers, kGraphicsShaderStageNum> PreviousConstantBuffers_;
  std::array<std::vector<ConstantBuffers>, kGraphicsShaderStageNum> ConstantBuffers_;

  VertexBuffer CurrentVertexBuffer_;
  IndexBuffer CurrentIndexBuffer_;
  vdl::InputLayoutType CurrentInputLayout_;
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
  std::array<ShaderResources, kGraphicsShaderStageNum> CurrentShaderResources_;
  std::array<Samplers, kGraphicsShaderStageNum> CurrentSamplers_;
  std::array<ConstantBuffers, kGraphicsShaderStageNum> CurrentConstantBuffers_;
private:
  template<ShaderType Type> void UpdateAndSetConstantBuffer();
  template<ShaderType Type> void SetShaderObjects();
protected:
  void PushGraphicsCommand();
  void Sort();
protected:
  void Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState,
    vdl::RasterizerState&& _RasterizerState, vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, InstanceBuffer&& _InstanceBuffer);
public:
  BaseGraphicsCommandList() = default;

  ~BaseGraphicsCommandList() = default;

  [[nodiscard]] virtual bool HasDrawCommand()const { return !DrawDatas_.empty() || !DrawIndexedDatas_.empty(); }

  [[nodiscard]] virtual const GraphicsCommands& GetGraphicsCommands()const { return GraphicsCommands_; }

  [[nodiscard]] const InstanceDatas& GetInstanceDatas(vdl::uint _Index)const { return Instances_[_Index]; }

  [[nodiscard]] const DrawData& GetDrawData(vdl::uint _Index)const { return DrawDatas_[_Index]; }

  [[nodiscard]] const DrawIndexedData& GetDrawIndexedData(vdl::uint _Index)const { return DrawIndexedDatas_[_Index]; }

  [[nodiscard]] const InstanceBuffer& GetInstanceBuffer()const { return InstanceBuffer_; }

  [[nodiscard]] const VertexBuffer& GetVertexBuffer(vdl::uint _Index)const { return VertexBuffers_[_Index]; }

  [[nodiscard]] const IndexBuffer& GetIndexBuffer(vdl::uint _Index)const { return IndexBuffers_[_Index]; }

  [[nodiscard]] const vdl::InputLayoutType& GetInputLayout(vdl::uint _Index)const { return InputLayouts_[_Index]; }

  [[nodiscard]] const vdl::TopologyType& GetTopology(vdl::uint _Index)const { return Topologys_[_Index]; }

  [[nodiscard]] const vdl::Scissor& GetScissor(vdl::uint _Index)const { return Scissors_[_Index]; }

  [[nodiscard]] const vdl::Viewport& GetViewport(vdl::uint _Index)const { return Viewports_[_Index]; }

  [[nodiscard]] const vdl::BlendState& GetBlendState(vdl::uint _Index)const { return BlendStates_[_Index]; }

  [[nodiscard]] const vdl::DepthStencilState& GetDepthStencilState(vdl::uint _Index)const { return DepthStencilStates_[_Index]; }

  [[nodiscard]] const vdl::RasterizerState& GetRasterizerState(vdl::uint _Index)const { return RasterizerStates_[_Index]; }

  [[nodiscard]] const vdl::VertexShader& GetVertexShader(vdl::uint _Index)const { return VertexShaders_[_Index]; }

  [[nodiscard]] const vdl::HullShader& GetHullShader(vdl::uint _Index)const { return HullShaders_[_Index]; }

  [[nodiscard]] const vdl::DomainShader& GetDomainShader(vdl::uint _Index)const { return DomainShaders_[_Index]; }

  [[nodiscard]] const vdl::GeometryShader& GetGeometryShader(vdl::uint _Index)const { return GeometryShaders_[_Index]; }

  [[nodiscard]] const vdl::PixelShader& GetPixelShader(vdl::uint _Index)const { return PixelShaders_[_Index]; }

  template<ShaderType Type>
  [[nodiscard]] const ShaderResources& GetShaderResources(vdl::uint _Index)const { return ShaderResources_[static_cast<vdl::uint>(Type)][_Index]; }

  template<ShaderType Type>
  [[nodiscard]] const Samplers& GetSamplers(vdl::uint _Index)const { return Samplers_[static_cast<vdl::uint>(Type)][_Index]; }

  template<ShaderType Type>
  [[nodiscard]] const ConstantBuffers& GetConstantBuffers(vdl::uint _Index)const { return ConstantBuffers_[static_cast<vdl::uint>(Type)][_Index]; }

  [[nodiscard]] const vdl::InputLayoutType& GetCurrentInputLayout()const { return CurrentInputLayout_; }

  [[nodiscard]] const vdl::TopologyType& GetCurrentTopology()const { return CurrentTopology_; }

  [[nodiscard]] const vdl::Scissor& GetCurrentScissor()const { return CurrentScissor_; }

  [[nodiscard]] const vdl::Viewport& GetCurrentViewport()const { return CurrentViewport_; }

  [[nodiscard]] const vdl::BlendState& GetCurrentBlendState()const { return CurrentBlendState_; }

  [[nodiscard]] const vdl::DepthStencilState& GetCurrentDepthStencilState()const { return CurrentDepthStencilState_; }

  [[nodiscard]] const vdl::RasterizerState& GetCurrentRasterizerState()const { return CurrentRasterizerState_; }

  [[nodiscard]] const vdl::VertexShader& GetCurrentVertexShader()const { return CurrentVertexShader_; }

  [[nodiscard]] const vdl::HullShader& GetCurrentHullShader()const { return CurrentHullShader_; }

  [[nodiscard]] const vdl::DomainShader& GetCurrentDomainShader()const { return CurrentDomainShader_; }

  [[nodiscard]] const vdl::GeometryShader& GetCurrentGeometryShader()const { return CurrentGeometryShader_; }

  [[nodiscard]] const vdl::PixelShader& GetCurrentPixelShader()const { return CurrentPixelShader_; }

  template<ShaderType Type>
  [[nodiscard]] const ShaderResources& GetCurrentShaderResources()const { return CurrentShaderResources_[static_cast<vdl::uint>(Type)]; }

  template<ShaderType Type>
  [[nodiscard]] const Samplers& GetCurrentSamplers()const { return CurrentSamplers_[static_cast<vdl::uint>(Type)]; }

  template<ShaderType Type>
  [[nodiscard]] const ConstantBuffers& GetCurrentConstantBuffers()const { return CurrentConstantBuffers_[static_cast<vdl::uint>(Type)]; }
};

class WrapGraphicsCommandList : public BaseGraphicsCommandList
{
protected:
  ThreadPool* pThreadPool_;
protected:
  mutable std::shared_mutex ResetMutex_;
  mutable std::shared_mutex Mutex_;
  bool isReady_ = false;
  std::condition_variable_any ConditionVariable_;
  std::function<void()> ResetFunc_;
protected:
  void Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState,
    vdl::RasterizerState&& _RasterizerState, vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, InstanceBuffer&& _InstanceBuffer);
public:
  WrapGraphicsCommandList() = default;

  void Reset();

  [[nodiscard]] bool HasDrawCommand()const override
  {
    std::scoped_lock Lock(ResetMutex_, Mutex_);
    return isReady_ && (!DrawDatas_.empty() || !DrawIndexedDatas_.empty());
  }

  [[nodiscard]] const GraphicsCommands& GetGraphicsCommands()const override
  {
    std::scoped_lock Lock(ResetMutex_, Mutex_);
    return GraphicsCommands_;
  }

  void SetVertexBuffer(const VertexBuffer& _VertexBuffer);

  void SetIndexBuffer(const IndexBuffer& _IndexBuffer);

  void SetInputLayout(vdl::InputLayoutType _InputLayout);

  void SetTopology(vdl::TopologyType _Topology);

  void SetScissor(const vdl::Scissor& _Scissor);

  void SetViewport(const vdl::Viewport& _Viewport);

  void SetBlendState(const vdl::BlendState& _BlendState);

  void SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState);

  void SetRasterizerState(const vdl::RasterizerState& _RasterizerState);

  void SetVertexShader(const vdl::VertexShader& _VertexShader);

  void SetHullShader(const vdl::HullShader& _HullShader);

  void SetDomainShader(const vdl::DomainShader& _DomainShader);

  void SetGeometryShader(const vdl::GeometryShader& _GeometryShader);

  void SetPixelShader(const vdl::PixelShader& _PixelShader);

  template<ShaderType Type> void SetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]);

  template<ShaderType Type> void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]);

  template<ShaderType Type> void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]);
};

//  メッシュ用コマンドリスト
template<class DisplayObject, class InstanceData>
class GraphicsCommandList : public WrapGraphicsCommandList
{
  static constexpr vdl::uint kInstanceSize = static_cast<vdl::uint>(sizeof(InstanceData));
private:
  IModelManager* pModelManager_;
private:
  void SetVertexBuffer(const VertexBuffer&);

  void SetIndexBuffer(const IndexBuffer&);
public:
  GraphicsCommandList() = default;

  void Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState,
    vdl::RasterizerState&& _RasterizerState, vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, InstanceBuffer&& _InstanceBuffer);

  void SetDrawData(const DisplayObject& _DisplayObject, InstanceData&& _InstanceData);
};

//  テクスチャ用コマンドリスト
template<class InstanceData>
class GraphicsCommandList<vdl::Texture, InstanceData> : public WrapGraphicsCommandList
{
  static constexpr vdl::uint kInstanceSize = static_cast<vdl::uint>(sizeof(InstanceData));
private:
  void SetVertexBuffer(const VertexBuffer&);

  void SetIndexBuffer(const IndexBuffer&);
public:
  GraphicsCommandList() = default;

  void Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState, vdl::RasterizerState&& _RasterizerState,
    vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, VertexBuffer&& _VertexBuffer, InstanceBuffer&& _InstanceBuffer);

  void SetDrawData(const vdl::Texture& _Texture, InstanceData&& _InstanceData);
};

//  GUI用コマンドリスト
template<>
class GraphicsCommandList<vdl::Texture, std::nullptr_t> : public WrapGraphicsCommandList
{
public:
  GraphicsCommandList() = default;

  void Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState,
    vdl::RasterizerState&& _RasterizerState, vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader);

  void SetDrawData(const vdl::Texture& _Texture, DrawIndexedData&& _DrawIndexedData);
};

//  頂点バッファ&インスタンスバッファ無し用コマンドリスト
template<>
class GraphicsCommandList<std::nullptr_t, std::nullptr_t> : public WrapGraphicsCommandList
{
private:
  void SetVertexBuffer(const VertexBuffer&);

  void SetIndexBuffer(const IndexBuffer&);
public:
  GraphicsCommandList() = default;

  void Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState, vdl::RasterizerState&& _RasterizerState,
    vdl::Sampler&& _Sampler);

  void SetDrawData(DrawData&& _DrawData);
};

#include "GraphicsCommandList.inl"
