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

class IBufferManager;

enum class RendererCommnadType
{
  eDraw = 0,
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
  eSetVertexStageSampler,
  eSetHullStageSampler,
  eSetDomainStageSampler,
  eSetGeometryStageSampler,
  eSetPixelStageSampler,
  eSetVertexStageTexture,
  eSetHullStageTexture,
  eSetDomainStageTexture,
  eSetGeometryStageTexture,
  eSetPixelStageTexture,
  eSetVertexStageConstantBuffer,
  eSetHullStageConstantBuffer,
  eSetDomainStageConstantBuffer,
  eSetGeometryStageConstantBuffer,
  eSetPixelStageConstantBuffer,
};
using RendererCommands = std::vector<std::pair<RendererCommnadType, vdl::uint>>;

class StateChangeFlags
{
  vdl::uint32_t Flags_;
public:
  StateChangeFlags() = default;

  void Clear() { Flags_ = 0; }

  bool Has(RendererCommnadType _Command)const { return Flags_ & (1 << static_cast<vdl::uint>(_Command)); }

  void Set(RendererCommnadType _Command) { Flags_ |= 1 << static_cast<vdl::uint>(_Command); }

  void Cancel(RendererCommnadType _Command) { Flags_ &= ~(1 << static_cast<vdl::uint>(_Command)); }
};

template<class DisplayObject, class InstanceData>
class RendererCommandList
{
  static constexpr vdl::uint kShaderTypes = static_cast<vdl::uint>(ShaderType::eGraphicsNum);
  static_assert(std::is_same<DisplayObject, vdl::Texture>::value || std::is_same<DisplayObject, vdl::StaticMesh>::value || std::is_same<DisplayObject, vdl::SkinnedMesh>::value);
private:
  using Samplers = std::vector<vdl::Sampler>;
  using TextureIDs = std::vector<vdl::ID>;
  using ConstantBufferIDs = std::vector<vdl::ID>;
private:
  IBufferManager* pBufferManager_;
private:
  RendererCommands RendererCommands_;

  StateChangeFlags StateChangeFlags_;

  std::vector<InstanceData> Instances_;

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
  std::array<std::vector<Samplers>, kShaderTypes> Samplers_;
  std::array<std::vector<TextureIDs>, kShaderTypes> TextureIDs_;
  std::array<ConstantBufferIDs, kShaderTypes> LastConstantBufferIDs_;
  std::array<std::vector<ConstantBufferIDs>, kShaderTypes> ConstantBufferIDs_;

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
  std::array<Samplers, kShaderTypes> CurrentSamplers_;
  std::array<TextureIDs, kShaderTypes> CurrentTextureIDs_;
  std::array<ConstantBufferIDs, kShaderTypes> CurrentConstantBufferIDs_;

  std::unordered_map<vdl::ID, DisplayObject> ReservedDisplayObjects_;
  std::unordered_map<vdl::ID, vdl::Texture> ReservedTextures_;
  std::unordered_map<vdl::ID, vdl::Detail::ConstantBufferData> ReservedConstantBuffers_;
public:
  RendererCommandList() = default;

  void Initialize(const vdl::uint2& _WindowSize, const vdl::BlendState& _BlendState, const vdl::DepthStencilState& _DepthStencilState, const vdl::RasterizerState& _RasterizerState,
    const vdl::VertexShader& _VertexShader, const vdl::HullShader& _HullShader, const vdl::DomainShader& _DomainShader, const vdl::GeometryShader& _GeometryShader, const vdl::PixelShader& _PixelShader);

  void Reset();

  const RendererCommands& GetList()const { return RendererCommands_; }

  void PushDrawData(const DisplayObject& _DisplayObject, InstanceData&& _InstanceData);
  const InstanceData& GetInstanceData(vdl::uint _Index)const { return Instances_[_Index]; }

  void PushScissor(const vdl::Scissor& _Scissor);
  const vdl::Scissor& GetScissor(vdl::uint _Index)const { return Scissors_[_Index]; }

  void PushViewport(const vdl::Viewport& _Viewport);
  const vdl::Viewport& GetViewport(vdl::uint _Index)const { return Viewports_[_Index]; }

  void PushBlendState(const vdl::BlendState& _BlendState);
  const vdl::BlendState& GetBlendState(vdl::uint _Index)const { return BlendStates_[_Index]; }

  void PushDepthStencilState(const vdl::DepthStencilState& _DepthStencilState);
  const vdl::DepthStencilState& GetDepthStencilState(vdl::uint _Index)const { return DepthStencilStates_[_Index]; }

  void PushRasterizerState(const vdl::RasterizerState& _RasterizerState);
  const vdl::RasterizerState& GetRasterizerState(vdl::uint _Index)const { return RasterizerStates_[_Index]; }

  void PushVertexShader(const vdl::VertexShader& _VertexShader);
  const vdl::VertexShader& GetVertexShader(vdl::uint _Index)const { return VertexShaders_[_Index]; }

  void PushHullShader(const vdl::HullShader& _HullShader);
  const vdl::HullShader& GetHullShader(vdl::uint _Index)const { return HullShaders_[_Index]; }

  void PushDomainShader(const vdl::DomainShader& _DomainShader);
  const vdl::DomainShader& GetDomainShader(vdl::uint _Index)const { return DomainShaders_[_Index]; }

  void PushGeometryShader(const vdl::GeometryShader& _GeometryShader);
  const vdl::GeometryShader& GetGeometryShader(vdl::uint _Index)const { return GeometryShaders_[_Index]; }

  void PushPixelShader(const vdl::PixelShader& _PixelShader);
  const vdl::PixelShader& GetPixelShader(vdl::uint _Index)const { return PixelShaders_[_Index]; }

  void PushVertexStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot);
  const vdl::Sampler& GetVertexStageSampler(vdl::uint _Index, vdl::uint _Slot)const { return Samplers_[static_cast<vdl::uint>(ShaderType::eVertexShader)][_Index][_Slot]; }

  void PushHullStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot);
  const vdl::Sampler& GetHullStageSampler(vdl::uint _Index, vdl::uint _Slot)const { return Samplers_[static_cast<vdl::uint>(ShaderType::eHullShader)][_Index][_Slot]; }

  void PushDomainStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot);
  const vdl::Sampler& GetDomainStageSampler(vdl::uint _Index, vdl::uint _Slot)const { return Samplers_[static_cast<vdl::uint>(ShaderType::eDomainShader)][_Index][_Slot]; }

  void PushGeometryStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot);
  const vdl::Sampler& GetGeometryStageSampler(vdl::uint _Index, vdl::uint _Slot)const { return Samplers_[static_cast<vdl::uint>(ShaderType::eGeometryShader)][_Index][_Slot]; }

  void PushPixelStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot);
  const vdl::Sampler& GetPixelStageSampler(vdl::uint _Index, vdl::uint _Slot)const { return Samplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][_Index][_Slot]; }

  void PushVertexStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot);
  const vdl::Texture& GetVertexStageTexture(vdl::uint _Index, vdl::uint _Slot)const { return ReservedTextures_.at(TextureIDs_[static_cast<vdl::uint>(ShaderType::eVertexShader)][_Index][_Slot]); }

  void PushHullStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot);
  const vdl::Texture& GetHullStageTexture(vdl::uint _Index, vdl::uint _Slot)const { return ReservedTextures_.at(TextureIDs_[static_cast<vdl::uint>(ShaderType::eHullShader)][_Index][_Slot]); }

  void PushDomainStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot);
  const vdl::Texture& GetDomainStageTexture(vdl::uint _Index, vdl::uint _Slot)const { return ReservedTextures_.at(TextureIDs_[static_cast<vdl::uint>(ShaderType::eDomainShader)][_Index][_Slot]); }

  void PushGeometryStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot);
  const vdl::Texture& GetGeometryStageTexture(vdl::uint _Index, vdl::uint _Slot)const { return ReservedTextures_.at(TextureIDs_[static_cast<vdl::uint>(ShaderType::eGeometryShader)][_Index][_Slot]); }

  void PushPixelStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot);
  const vdl::Texture& GetPixelStageTexture(vdl::uint _Index, vdl::uint _Slot)const { return ReservedTextures_.at(TextureIDs_[static_cast<vdl::uint>(ShaderType::ePixelShader)][_Index][_Slot]); }

  void PushVertexStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot);
  const vdl::Detail::ConstantBufferData& GetVertexStageConstantBuffer(vdl::uint _Index, vdl::uint _Slot)const { return ReservedConstantBuffers_.at(ConstantBufferIDs_[static_cast<vdl::uint>(ShaderType::eVertexShader)][_Index][_Slot]); }

  void PushHullStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot);
  const vdl::Detail::ConstantBufferData& GetHullStageConstantBuffer(vdl::uint _Index, vdl::uint _Slot)const { return ReservedConstantBuffers_.at(ConstantBufferIDs_[static_cast<vdl::uint>(ShaderType::eHullShader)][_Index][_Slot]); }

  void PushDomainStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot);
  const vdl::Detail::ConstantBufferData& GetDomainStageConstantBuffer(vdl::uint _Index, vdl::uint _Slot)const { return ReservedConstantBuffers_.at(ConstantBufferIDs_[static_cast<vdl::uint>(ShaderType::eDomainShader)][_Index][_Slot]); }

  void PushGeometryStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot);
  const vdl::Detail::ConstantBufferData& GetGeometryStageConstantBuffer(vdl::uint _Index, vdl::uint _Slot)const { return ReservedConstantBuffers_.at(ConstantBufferIDs_[static_cast<vdl::uint>(ShaderType::eGeometryShader)][_Index][_Slot]); }

  void PushPixelStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot);
  const vdl::Detail::ConstantBufferData& GetPixelStageConstantBuffer(vdl::uint _Index, vdl::uint _Slot)const { return ReservedConstantBuffers_.at(ConstantBufferIDs_[static_cast<vdl::uint>(ShaderType::ePixelShader)][_Index][_Slot]); }
};

#include "RendererCommand.inl"
