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
#include <vdl/Constants/Constants.hpp>

#include <array>
#include <vector>
#include <unordered_map>

enum class RendererCommandFlag : vdl::uint32_t
{
  eDraw = 1 << 0,
  eSetVertexBuffer = 1 << 1,
  eSetInstanceBuffer = 1 << 2,
  eSetIndexBuffer = 1 << 3,
  eSetInputLayout = 1 << 4,
  eSetTopology = 1 << 5,
  eSetScissor = 1 << 6,
  eSetViewport = 1 << 7,
  eSetBlendState = 1 << 8,
  eSetDepthStencilState = 1 << 9,
  eSetRasterizerState = 1 << 10,
  eSetVertexShader = 1 << 11,
  eSetHullShader = 1 << 12,
  eSetDomainShader = 1 << 13,
  eSetGeometryShader = 1 << 14,
  eSetPixelShader = 1 << 15,
  eSetVertexStageShaderResource = 1 << 16,
  eSetHullStageShaderResource = 1 << 17,
  eSetDomainStageShaderResource = 1 << 18,
  eSetGeometryStageShaderResource = 1 << 19,
  eSetPixelStageShaderResource = 1 << 20,
  eSetVertexStageSampler = 1 << 21,
  eSetHullStageSampler = 1 << 22,
  eSetDomainStageSampler = 1 << 23,
  eSetGeometryStageSampler = 1 << 24,
  eSetPixelStageSampler = 1 << 25,
  eSetVertexStageConstantBuffer = 1 << 26,
  eSetHullStageConstantBuffer = 1 << 27,
  eSetDomainStageConstantBuffer = 1 << 28,
  eSetGeometryStageConstantBuffer = 1 << 29,
  eSetPixelStageConstantBuffer = 1 << 30,
};
using RendererCommandFlags = vdl::Flags<RendererCommandFlag, vdl::uint32_t>;

class IBufferManager;
class IModelManager;

struct DrawData
{
  vdl::uint VertexCount;
};
struct DrawIndexedData
{
  vdl::uint FirstIndex;
  vdl::uint IndexCount;
};

class BaseRendererCommandList
{
  using RendererCommandPair = std::pair<RendererCommandFlags, vdl::uint>;
  using RendererCommands = std::vector<RendererCommandPair>;
  using ShaderResources = std::vector<vdl::ShaderResource>;
  using Samplers = std::vector<vdl::Sampler>;
  using ConstantBuffers = std::vector<vdl::Detail::ConstantBufferData>;
protected:
  static constexpr RendererCommandFlags kDrawFlag = RendererCommandFlag::eDraw;
  static constexpr RendererCommandFlags kSetVertexBufferFlag = RendererCommandFlag::eSetVertexBuffer;
  static constexpr RendererCommandFlags kSetInstanceBufferFlag = RendererCommandFlag::eSetInstanceBuffer;
  static constexpr RendererCommandFlags kSetIndexBufferFlag = RendererCommandFlag::eSetIndexBuffer;
  static constexpr RendererCommandFlags kSetInputLayoutFlag = RendererCommandFlag::eSetInputLayout;
  static constexpr RendererCommandFlags kSetTopologyFlag = RendererCommandFlag::eSetTopology;
  static constexpr RendererCommandFlags kSetScissorFlag = RendererCommandFlag::eSetScissor;
  static constexpr RendererCommandFlags kSetViewportFlag = RendererCommandFlag::eSetViewport;
  static constexpr RendererCommandFlags kSetBlendStateFlag = RendererCommandFlag::eSetBlendState;
  static constexpr RendererCommandFlags kSetDepthStencilStateFlag = RendererCommandFlag::eSetDepthStencilState;
  static constexpr RendererCommandFlags kSetRasterizerStateFlag = RendererCommandFlag::eSetRasterizerState;
  static constexpr RendererCommandFlags kSetVertexShaderFlag = RendererCommandFlag::eSetVertexShader;
  static constexpr RendererCommandFlags kSetHullShaderFlag = RendererCommandFlag::eSetHullShader;
  static constexpr RendererCommandFlags kSetDomainShaderFlag = RendererCommandFlag::eSetDomainShader;
  static constexpr RendererCommandFlags kSetGeometryShaderFlag = RendererCommandFlag::eSetGeometryShader;
  static constexpr RendererCommandFlags kSetPixelShaderFlag = RendererCommandFlag::eSetPixelShader;
  static constexpr RendererCommandFlags kSetVertexStageShaderResourceFlag = RendererCommandFlag::eSetVertexStageShaderResource;
  static constexpr RendererCommandFlags kSetHullStageShaderResourceFlag = RendererCommandFlag::eSetHullStageShaderResource;
  static constexpr RendererCommandFlags kSetDomainStageShaderResourceFlag = RendererCommandFlag::eSetDomainStageShaderResource;
  static constexpr RendererCommandFlags kSetGeometryStageShaderResourceFlag = RendererCommandFlag::eSetGeometryStageShaderResource;
  static constexpr RendererCommandFlags kSetPixelStageShaderResourceFlag = RendererCommandFlag::eSetPixelStageShaderResource;
  static constexpr RendererCommandFlags kSetVertexStageSamplerFlag = RendererCommandFlag::eSetVertexStageSampler;
  static constexpr RendererCommandFlags kSetHullStageSamplerFlag = RendererCommandFlag::eSetHullStageSampler;
  static constexpr RendererCommandFlags kSetDomainStageSamplerFlag = RendererCommandFlag::eSetDomainStageSampler;
  static constexpr RendererCommandFlags kSetGeometryStageSamplerFlag = RendererCommandFlag::eSetGeometryStageSampler;
  static constexpr RendererCommandFlags kSetPixelStageSamplerFlag = RendererCommandFlag::eSetPixelStageSampler;
  static constexpr RendererCommandFlags kSetVertexStageConstantBufferFlag = RendererCommandFlag::eSetVertexStageConstantBuffer;
  static constexpr RendererCommandFlags kSetHullStageConstantBufferFlag = RendererCommandFlag::eSetHullStageConstantBuffer;
  static constexpr RendererCommandFlags kSetDomainStageConstantBufferFlag = RendererCommandFlag::eSetDomainStageConstantBuffer;
  static constexpr RendererCommandFlags kSetGeometryStageConstantBufferFlag = RendererCommandFlag::eSetGeometryStageConstantBuffer;
  static constexpr RendererCommandFlags kSetPixelStageConstantBufferFlag = RendererCommandFlag::eSetPixelStageConstantBuffer;
protected:
  IBufferManager* pBufferManager_;
protected:
  RendererCommands RendererCommands_;
  RendererCommandFlags RendererCommandFlags_;

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
  void PushRendererCommand();
public:
  BaseRendererCommandList() = default;

  ~BaseRendererCommandList() = default;

  virtual void Reset() = 0;

  [[nodiscard]] virtual bool HasDrawCommand()const = 0;

  [[nodiscard]] const RendererCommands& GetRendererCommands()const { return RendererCommands_; }

  [[nodiscard]] virtual vdl::uint GetInstanceSize()const = 0;

  [[nodiscard]] virtual const DrawData& GetDrawData(vdl::uint _Index)const = 0;

  [[nodiscard]] virtual const DrawIndexedData& GetDrawIndexedData(vdl::uint _Index)const = 0;

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

  template<ShaderType Type> [[nodiscard]] const ShaderResources& GetShaderResources(vdl::uint _Index)const { return ShaderResources_[static_cast<vdl::uint>(Type)][_Index]; }

  template<ShaderType Type> [[nodiscard]] const Samplers& GetSamplers(vdl::uint _Index)const { return Samplers_[static_cast<vdl::uint>(Type)][_Index]; }

  template<ShaderType Type> [[nodiscard]] const ConstantBuffers& GetConstantBuffers(vdl::uint _Index)const { return ConstantBuffers_[static_cast<vdl::uint>(Type)][_Index]; }

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

  template<ShaderType Type> [[nodiscard]] const ShaderResources& GetCurrentShaderResources()const { return CurrentShaderResources_[static_cast<vdl::uint>(Type)]; }

  template<ShaderType Type> [[nodiscard]] const Samplers& GetCurrentSamplers()const { return CurrentSamplers_[static_cast<vdl::uint>(Type))]; }

  template<ShaderType Type> [[nodiscard]] const ConstantBuffers& GetCurrentConstantBuffers()const { return CurrentConstantBuffers_[static_cast<vdl::uint>(Type)]; }

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

template<class DisplayObject, class InstanceData>
class RendererCommandList : public BaseRendererCommandList
{
  IModelManager* pModelManager_;
private:
  std::vector<vdl::ID> DisplayObjectIDs_;
  std::vector<InstanceData> Instances_;
  std::vector<DrawIndexedData> DrawIndexedDatas_;
  std::unordered_map<vdl::ID, DisplayObject> ReservedDisplayObjects_;
public:
  RendererCommandList() = default;

  void Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState, vdl::RasterizerState&& _RasterizerState,
    vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, InstanceBuffer&& _InstanceBuffer);

  void Reset()override;

  void SetDrawData(const DisplayObject& _DisplayObject, InstanceData&& _InstanceData);

  bool HasDrawCommand()const override { return !DrawIndexedDatas_.empty(); }

  vdl::uint GetInstanceSize()const override { return static_cast<vdl::uint>(sizeof(InstanceData)); }

  const DrawData& GetDrawData(vdl::uint _Index)const override { return {}; }

  const DrawIndexedData& GetDrawIndexedData(vdl::uint _Index)const override { return DrawIndexedDatas_[_Index]; }
};

class RendererCommandList<std::nullptr_t, std::nullptr_t> : public BaseRendererCommandList
{
  std::vector<DrawData> DrawDatas_;
public:
  RendererCommandList() = default;

  void Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState, vdl::RasterizerState&& _RasterizerState,
    vdl::Sampler&& _Sampler, InstanceBuffer&& _InstanceBuffer);

  void Reset()override;

  void SetDrawData(vdl::uint _VertexCount);

  bool HasDrawCommand()const override { return !DrawDatas_.empty(); }

  vdl::uint GetInstanceSize()const override { return 0; }

  const DrawData& GetDrawData(vdl::uint _Index)const override { return DrawDatas_[_Index]; }

  const DrawIndexedData& GetDrawIndexedData(vdl::uint _Index)const override { return {}; }
};

template<class InstanceData>
class RendererCommandList<vdl::Texture, InstanceData> : public BaseRendererCommandList
{
  std::vector<vdl::ID> DisplayObjectIDs_;
  std::vector<InstanceData> Instances_;
  std::vector<DrawData> DrawDatas_;
  std::unordered_map<vdl::ID, vdl::Texture> ReservedTextures_;
public:
  RendererCommandList() = default;

  void Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState, vdl::RasterizerState&& _RasterizerState,
    vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, VertexBuffer&& _VertexBuffer, InstanceBuffer&& _InstanceBuffer);

  void Reset()override;

  void SetDrawData(const vdl::Texture& _Texture, InstanceData&& _InstanceData);

  bool HasDrawCommand()const override { return !Instances_.empty(); }

  vdl::uint GetInstanceSize()const override { return static_cast<vdl::uint>(sizeof(InstanceData)); }

  const DrawData& GetDrawData(vdl::uint _Index)const override { return DrawDatas_[_Index]; }

  const DrawIndexedData& GetDrawIndexedData(vdl::uint _Index)const override { return {}; }
};

#include "RendererCommandList.inl"
