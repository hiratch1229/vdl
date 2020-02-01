#include <vdl/Engine.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/ModelManager/IModelManager.hpp>

#define SetState(StateName)\
if (Current##StateName##_ == _##StateName)\
{\
  return;\
}\
\
RendererCommandFlags_ |= kSet##StateName##Flag;\
Current##StateName##_ = _##StateName;
inline void BaseRendererCommandList::SetInputLayout(vdl::InputLayoutType _InputLayout)
{
  SetState(InputLayout)
}

inline void BaseRendererCommandList::SetTopology(vdl::TopologyType _Topology)
{
  SetState(Topology)
}

inline void BaseRendererCommandList::SetScissor(const vdl::Scissor& _Scissor)
{
  SetState(Scissor)
}

inline void BaseRendererCommandList::SetViewport(const vdl::Viewport& _Viewport)
{
  SetState(Viewport)
}

inline void BaseRendererCommandList::SetBlendState(const vdl::BlendState& _BlendState)
{
  SetState(BlendState)
}

inline void BaseRendererCommandList::SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState)
{
  SetState(DepthStencilState)
}

inline void BaseRendererCommandList::SetRasterizerState(const vdl::RasterizerState& _RasterizerState)
{
  SetState(RasterizerState)
}

inline void BaseRendererCommandList::SetVertexShader(const vdl::VertexShader& _VertexShader)
{
  SetState(VertexShader)
}

inline void BaseRendererCommandList::SetHullShader(const vdl::HullShader& _HullShader)
{
  SetState(HullShader)
}

inline void BaseRendererCommandList::SetDomainShader(const vdl::DomainShader& _DomainShader)
{
  SetState(DomainShader)
}

inline void BaseRendererCommandList::SetGeometryShader(const vdl::GeometryShader& _GeometryShader)
{
  SetState(GeometryShader)
}

inline void BaseRendererCommandList::SetPixelShader(const vdl::PixelShader& _PixelShader)
{
  SetState(PixelShader)
}
#undef SetState

#define SetShaderState(StateName)\
constexpr RendererCommandFlags kSet##StateName##Flag = kSetVertexStage##StateName##Flag << static_cast<vdl::uint>(Type);\
\
StateName##s& Current##StateName##s = Current##StateName##s_[static_cast<vdl::uint>(Type)];\
\
if (const vdl::uint RequiredSize = _StartSlot + _##StateName##Num;\
Current##StateName##s_.size() < RequiredSize)\
{\
  Current##StateName##s_.resize(static_cast<size_t>(RequiredSize));\
  RendererCommandFlags_ |= kSet##StateName##Flag;\
}\
\
for (vdl::uint StateName##Count = 0; StateName##Count < _##StateName##Num; ++StateName##Count)\
{\
  auto& Current##StateName = Current##StateName##s[_StartSlot + StateName##Count];\
  const auto& StateName = _##StateName##s[StateName##Count];\
  \
  if (Current##StateName == StateName)\
  {\
    continue;\
  }\
  \
  RendererCommandFlags_ |= kSet##StateName##Flag;\
  Current##StateName = StateName;\
}
template<ShaderType Type>
inline void BaseRendererCommandList::SetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  SetShaderState(ShaderResource)
}

template<ShaderType Type>
inline void BaseRendererCommandList::SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetShaderState(Sampler)
}

template<ShaderType Type>
inline void BaseRendererCommandList::SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetShaderState(ConstantBuffer)
}
#undef SetShaderState

template<ShaderType Type>
void BaseRendererCommandList::UpdateAndSetConstantBuffer()
{
  constexpr RendererCommandFlags kSetConstantBufferFlag = kSetVertexStageConstantBufferFlag << static_cast<vdl::uint>(Type);

  ConstantBuffers TempConstantBuffers;
  const ConstantBuffers& CurrentConstantBuffers = CurrentConstantBuffers_[static_cast<vdl::uint>(Type)];
  ConstantBuffers& PreviousConstantBuffers = PreviousConstantBuffers_[static_cast<vdl::uint>(Type)];
  std::vector<ConstantBuffers>& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(Type)];

  TempConstantBuffers = ConstantBuffers.back();
  {
    const vdl::uint CurrentConstantBufferNum = static_cast<vdl::uint>(CurrentConstantBuffers.size());
    const vdl::uint PreviousConstantBufferNum = static_cast<vdl::uint>(PreviousConstantBuffers.size());

    vdl::uint ConstantBufferCount;

    //  サイズを最新のものと同じにする
    if (PreviousConstantBufferNum < CurrentConstantBufferNum)
    {
      TempConstantBuffers.resize(static_cast<size_t>(CurrentConstantBufferNum));
      RendererCommandFlags_ |= kSetConstantBufferFlag;

      //  サイズが増えた部分のデータのコピー
      for (ConstantBufferCount = PreviousConstantBufferNum; ConstantBufferCount < CurrentConstantBufferNum; ++ConstantBufferCount)
      {
        const vdl::Detail::ConstantBufferData& CurrentConstantBuffer = CurrentConstantBuffers[ConstantBufferCount];

        if (CurrentConstantBuffer.isEmpty())
        {
          continue;
        }
        TempConstantBuffers[ConstantBufferCount] = pBufferManager_->CloneConstantBuffer(CurrentConstantBuffer);
      }
    }

    for (ConstantBufferCount = 0; ConstantBufferCount < PreviousConstantBufferNum; ++ConstantBufferCount)
    {
      const vdl::Detail::ConstantBufferData& CurrentConstantBuffer = CurrentConstantBuffers[ConstantBufferCount];
      const vdl::Detail::ConstantBufferData& PreviousConstantBuffer = PreviousConstantBuffers[ConstantBufferCount];
      vdl::Detail::ConstantBufferData& TempConstantBuffer = TempConstantBuffers[ConstantBufferCount];

      //  データの更新が不要なら次へ
      if (CurrentConstantBuffer.isEmpty()
        || (CurrentConstantBuffer == PreviousConstantBuffer && ::memcmp(CurrentConstantBuffer.GetData(), TempConstantBuffer.GetData(), CurrentConstantBuffer.GetBufferSize()) == 0))
      {
        continue;
      }

      TempConstantBuffer = pBufferManager_->CloneConstantBuffer(CurrentConstantBuffer);
      RendererCommandFlags_ |= kSetConstantBufferFlag;
    }
  }

  if (RendererCommandFlags_ & kSetConstantBufferFlag)
  {
    RendererCommands_.emplace_back(kSetConstantBufferFlag, static_cast<vdl::uint>(ConstantBuffers.size()));
    PreviousConstantBuffers = CurrentConstantBuffers;
    ConstantBuffers.emplace_back(std::move(TempConstantBuffers));
    RendererCommandFlags_ &= ~kSetConstantBufferFlag;
  }
}

template<ShaderType Type>
void BaseRendererCommandList::SetShaderObjects()
{
  //  SetShaderResource
  {
    constexpr RendererCommandFlags kSetShaderResourceFlag = kSetVertexStageShaderResourceFlag << static_cast<vdl::uint>(Type);
    if (RendererCommandFlags_ & kSetShaderResourceFlag)
    {
      auto& ShaderResources = ShaderResources_[static_cast<vdl::uint>(Type)];

      RendererCommands_.emplace_back(kSetShaderResourceFlag, static_cast<vdl::uint>(ShaderResources.size()));
      ShaderResources.push_back(CurrentShaderResources_[static_cast<vdl::uint>(Type)]);

      RendererCommandFlags_ &= ~kSetShaderResourceFlag;
    }
  }

  //  SetSampler
  {
    constexpr RendererCommandFlags kSetSamplerFlag = kSetVertexStageSamplerFlag << static_cast<vdl::uint>(Type);
    if (RendererCommandFlags_ & kSetSamplerFlag)
    {
      auto& Sampler = Samplers_[static_cast<vdl::uint>(Type)];

      RendererCommands_.emplace_back(kSetSamplerFlag, static_cast<vdl::uint>(Sampler.size()));
      Sampler.push_back(CurrentSamplers_[static_cast<vdl::uint>(Type)]);

      RendererCommandFlags_ &= ~kSetSamplerFlag;
    }
  }

  UpdateAndSetConstantBuffer<Type>();
}

void BaseRendererCommandList::PushRendererCommand()
{
  if (RendererCommandFlags_ & kSetInputLayoutFlag)
  {
    RendererCommands_.emplace_back(kSetInputLayoutFlag, static_cast<vdl::uint>(InputLayouts_.size()));
    InputLayouts_.push_back(CurrentInputLayout_);

    RendererCommandFlags_ &= ~kSetInputLayoutFlag;
  }

  if (RendererCommandFlags_ & kSetTopologyFlag)
  {
    RendererCommands_.emplace_back(kSetTopologyFlag, static_cast<vdl::uint>(Topologys_.size()));
    Topologys_.push_back(CurrentTopology_);

    RendererCommandFlags_ &= ~kSetTopologyFlag;
  }

  if (RendererCommandFlags_ & kSetScissorFlag)
  {
    RendererCommands_.emplace_back(kSetScissorFlag, static_cast<vdl::uint>(Scissors_.size()));
    Scissors_.push_back(CurrentScissor_);

    RendererCommandFlags_ &= ~kSetScissorFlag;
  }

  if (RendererCommandFlags_ & kSetViewportFlag)
  {
    RendererCommands_.emplace_back(kSetViewportFlag, static_cast<vdl::uint>(Viewports_.size()));
    Viewports_.push_back(CurrentViewport_);

    RendererCommandFlags_ &= ~kSetViewportFlag;
  }

  if (RendererCommandFlags_ & kSetBlendStateFlag)
  {
    RendererCommands_.emplace_back(kSetBlendStateFlag, static_cast<vdl::uint>(BlendStates_.size()));
    BlendStates_.push_back(CurrentBlendState_);

    RendererCommandFlags_ &= ~kSetBlendStateFlag;
  }

  if (RendererCommandFlags_ & kSetDepthStencilStateFlag)
  {
    RendererCommands_.emplace_back(kSetDepthStencilStateFlag, static_cast<vdl::uint>(DepthStencilStates_.size()));
    DepthStencilStates_.push_back(CurrentDepthStencilState_);

    RendererCommandFlags_ &= ~kSetDepthStencilStateFlag;
  }

  if (RendererCommandFlags_ & kSetRasterizerStateFlag)
  {
    RendererCommands_.emplace_back(kSetRasterizerStateFlag, static_cast<vdl::uint>(RasterizerStates_.size()));
    RasterizerStates_.push_back(CurrentRasterizerState_);

    RendererCommandFlags_ &= ~kSetRasterizerStateFlag;
  }

  if (RendererCommandFlags_ & kSetVertexShaderFlag)
  {
    RendererCommands_.emplace_back(kSetVertexShaderFlag, static_cast<vdl::uint>(VertexShaders_.size()));
    VertexShaders_.push_back(CurrentVertexShader_);

    RendererCommandFlags_ &= ~kSetVertexShaderFlag;
  }

  if (RendererCommandFlags_ & kSetHullShaderFlag)
  {
    RendererCommands_.emplace_back(kSetHullShaderFlag, static_cast<vdl::uint>(HullShaders_.size()));
    HullShaders_.push_back(CurrentHullShader_);

    RendererCommandFlags_ &= ~kSetHullShaderFlag;
  }

  if (RendererCommandFlags_ & kSetDomainShaderFlag)
  {
    RendererCommands_.emplace_back(kSetDomainShaderFlag, static_cast<vdl::uint>(DomainShaders_.size()));
    DomainShaders_.push_back(CurrentDomainShader_);

    RendererCommandFlags_ &= ~kSetDomainShaderFlag;
  }

  if (RendererCommandFlags_ & kSetGeometryShaderFlag)
  {
    RendererCommands_.emplace_back(kSetGeometryShaderFlag, static_cast<vdl::uint>(GeometryShaders_.size()));
    GeometryShaders_.push_back(CurrentGeometryShader_);

    RendererCommandFlags_ &= ~kSetGeometryShaderFlag;
  }

  if (RendererCommandFlags_ & kSetPixelShaderFlag)
  {
    RendererCommands_.emplace_back(kSetPixelShaderFlag, static_cast<vdl::uint>(PixelShaders_.size()));
    PixelShaders_.push_back(CurrentPixelShader_);

    RendererCommandFlags_ &= ~kSetPixelShaderFlag;
  }

  if (!CurrentVertexShader_.isEmpty())
  {
    SetShaderObjects<ShaderType::eVertexShader>();
  }

  if (!CurrentHullShader_.isEmpty())
  {
    SetShaderObjects<ShaderType::eHullShader>();
  }

  if (!CurrentDomainShader_.isEmpty())
  {
    SetShaderObjects<ShaderType::eDomainShader>();
  }

  if (!CurrentGeometryShader_.isEmpty())
  {
    SetShaderObjects<ShaderType::eGeometryShader>();
  }

  if (!CurrentPixelShader_.isEmpty())
  {
    SetShaderObjects<ShaderType::ePixelShader>();
  }
}

//--------------------------------------------------

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState, vdl::RasterizerState&& _RasterizerState,
  vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, InstanceBuffer&& _InstanceBuffer)
{
  pBufferManager_ = Engine::Get<IBufferManager>();
  pModelManager_ = Engine::Get<IModelManager>();

  InstanceBuffer_ = std::move(_InstanceBuffer);
  InputLayouts_.push_back(CurrentInputLayout_ = std::move(_InputLayout));
  Topologys_.push_back(CurrentTopology_ = std::move(_Topology));
  Scissors_.push_back(CurrentScissor_ = vdl::Scissor());
  Viewports_.push_back(CurrentViewport_ = vdl::Viewport());
  BlendStates_.push_back(CurrentBlendState_ = std::move(_BlendState));
  DepthStencilStates_.push_back(CurrentDepthStencilState_ = std::move(_DepthStencilState));
  RasterizerStates_.push_back(CurrentRasterizerState_ = std::move(_RasterizerState));
  VertexShaders_.push_back(CurrentVertexShader_ = std::move(_VertexShader));
  PixelShaders_.push_back(CurrentPixelShader_ = std::move(_PixelShader));
  HullShaders_.push_back(CurrentHullShader_ = vdl::HullShader());
  DomainShaders_.push_back(CurrentDomainShader_ = vdl::DomainShader());
  GeometryShaders_.push_back(CurrentGeometryShader_ = vdl::GeometryShader());

  for (auto& ShaderResources : ShaderResources_) { ShaderResources.resize(1); }
  for (auto& Samplers : Samplers_) { Samplers.resize(1); }
  for (auto& ConstantBuffers : ConstantBuffers_) { ConstantBuffers.resize(1); }

  Samplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][0].resize(1);
  CurrentSamplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)].resize(1);
  Samplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][0][0] = CurrentSamplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][0] = std::move(_Sampler);

  Reset();
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::Reset()
{
  RendererCommands_.clear();
  RendererCommandFlags_ = 0;
  DisplayObjectIDs_.clear();
  Instances_.clear();
  DrawIndexedDatas_.clear();
  ReservedDisplayObjects_.clear();

  VertexBuffers_.clear();
  IndexBuffers_.clear();
  RendererCommandFlags_ |= kSetIndexBufferFlag;

  if (Topologys_.size() > 1)
  {
    Topologys_ = { std::move(Topologys_.back()) };
  }
  RendererCommandFlags_ |= kSetTopologyFlag;

  if (Scissors_.size() > 1)
  {
    Scissors_ = { std::move(Scissors_.back()) };
  }
  RendererCommandFlags_ |= kSetScissorFlag;

  if (Viewports_.size() > 1)
  {
    Viewports_ = { std::move(Viewports_.back()) };
  }
  RendererCommandFlags_ |= kSetViewportFlag;

  if (BlendStates_.size() > 1)
  {
    BlendStates_ = { std::move(BlendStates_.back()) };
  }
  RendererCommandFlags_ |= kSetBlendStateFlag;

  if (DepthStencilStates_.size() > 1)
  {
    DepthStencilStates_ = { std::move(DepthStencilStates_.back()) };
  }
  RendererCommandFlags_ |= kSetDepthStencilFlag;

  if (RasterizerStates_.size() > 1)
  {
    RasterizerStates_ = { std::move(RasterizerStates_.back()) };
  }
  RendererCommandFlags_ |= kSetRasterizerStateFlag;

  if (VertexShaders_.size() > 1)
  {
    VertexShaders_ = { std::move(VertexShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetVertexShaderFlag;

  if (HullShaders_.size() > 1)
  {
    HullShaders_ = { std::move(HullShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetHullShaderFlag;

  if (DomainShaders_.size() > 1)
  {
    DomainShaders_ = { std::move(DomainShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetDomainShaderFlag;

  if (GeometryShaders_.size() > 1)
  {
    GeometryShaders_ = { std::move(GeometryShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetGeometryShaderFlag;

  if (PixelShaders_.size() > 1)
  {
    PixelShaders_ = { std::move(PixelShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetPixelShaderFlag;

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  {
    auto& ShaderResources = ShaderResources_[ShaderTypeCount];

    if (ShaderResources.size() > 1)
    {
      ShaderResources = { std::move(ShaderResources.back()) };
    }
    RendererCommandFlags_ |= kSetVertexStageShaderResourceFlag << ShaderTypeCount;
  }

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  {
    auto& Samplers = Samplers_[ShaderTypeCount];

    if (Samplers.size() > 1)
    {
      Samplers = { std::move(Samplers.back()) };
    }
    RendererCommandFlags_ |= kSetVertexStageSamplerFlag << ShaderTypeCount;
  }

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  {
    auto& ConstantBuffers = ConstantBuffers_[ShaderTypeCount];

    if (ConstantBuffers.size() > 1)
    {
      ConstantBuffers = { std::move(ConstantBuffers.back()) };
    }
    RendererCommandFlags_ |= kSetVertexStageConstantBufferFlag << ShaderTypeCount;
  }
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::SetDrawData(const DisplayObject& _DisplayObject, InstanceData&& _InstanceData)
{
  PushRendererCommand();

  //  Draw
  {
    const vdl::ID DiplayObjectID = _DisplayObject.GetID();

    const Mesh* pMesh = pModelManager_->GetMesh(DiplayObjectID);
    if (CurrentVertexBuffer_ != pMesh->VertexBuffer)
    {
      RendererCommands_.emplace_back(kSetVertexBufferFlag, static_cast<vdl::uint>(VertexBuffers_.size()));
      VertexBuffers_.push_back(CurrentVertexBuffer_ = pMesh->VertexBuffer);
    }
    if (CurrentIndexBuffer_ != pMesh->IndexBuffer)
    {
      RendererCommands_.emplace_back(kSetIndexBufferFlag, static_cast<vdl::uint>(IndexBuffers_.size()));
      IndexBuffers_.push_back(CurrentIndexBuffer_ = pMesh->IndexBuffer);
    }

    if (ReservedDisplayObjects_.find(DiplayObjectID) == ReservedDisplayObjects_.end())
    {
      ReservedDisplayObjects_.insert(std::make_pair(DiplayObjectID, _DisplayObject));
    }

    RendererCommands_.emplace_back(kDrawFlag, static_cast<vdl::uint>(Instances_.size()));
    DrawIndexedDatas_.emplace_back(pMesh->FirstIndex, pMesh->IndexCount);
    DisplayObjectIDs_.push_back(DiplayObjectID);
    Instances_.emplace_back(std::move(_InstanceData));
  }
}

//--------------------------------------------------

void RendererCommandList<std::nullptr_t, std::nullptr_t>::Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState, vdl::RasterizerState&& _RasterizerState,
  vdl::Sampler&& _Sampler, InstanceBuffer&& _InstanceBuffer)
{
  pBufferManager_ = Engine::Get<IBufferManager>();

  InstanceBuffer_ = std::move(_InstanceBuffer);
  InputLayouts_.push_back(CurrentInputLayout_ = std::move(_InputLayout));
  Topologys_.push_back(CurrentTopology_ = std::move(_Topology));
  Scissors_.push_back(CurrentScissor_ = vdl::Scissor());
  Viewports_.push_back(CurrentViewport_ = vdl::Viewport());
  BlendStates_.push_back(CurrentBlendState_ = std::move(_BlendState));
  DepthStencilStates_.push_back(CurrentDepthStencilState_ = std::move(_DepthStencilState));
  RasterizerStates_.push_back(CurrentRasterizerState_ = std::move(_RasterizerState));
  VertexShaders_.push_back(CurrentVertexShader_ = vdl::VertexShader());
  PixelShaders_.push_back(CurrentPixelShader_ = vdl::PixelShader());
  HullShaders_.push_back(CurrentHullShader_ = vdl::HullShader());
  DomainShaders_.push_back(CurrentDomainShader_ = vdl::DomainShader());
  GeometryShaders_.push_back(CurrentGeometryShader_ = vdl::GeometryShader());

  for (auto& ShaderResources : ShaderResources_) { ShaderResources.resize(1); }
  for (auto& Samplers : Samplers_) { Samplers.resize(1); }
  for (auto& ConstantBuffers : ConstantBuffers_) { ConstantBuffers.resize(1); }

  Samplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][0].resize(1);
  CurrentSamplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)].resize(1);
  Samplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][0][0] = CurrentSamplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][0] = std::move(_Sampler);

  Reset();
}

void RendererCommandList<std::nullptr_t, std::nullptr_t>::Reset()
{
  RendererCommands_.clear();
  RendererCommandFlags_ = 0;
  DrawDatas_.clear();

  RendererCommandFlags_ |= kSetInstanceBufferFlag;
  VertexBuffers_.clear();
  IndexBuffers_.clear();

  if (Topologys_.size() > 1)
  {
    Topologys_ = { std::move(Topologys_.back()) };
  }
  RendererCommandFlags_ |= kSetTopologyFlag;

  if (Scissors_.size() > 1)
  {
    Scissors_ = { std::move(Scissors_.back()) };
  }
  RendererCommandFlags_ |= kSetScissorFlag;

  if (Viewports_.size() > 1)
  {
    Viewports_ = { std::move(Viewports_.back()) };
  }
  RendererCommandFlags_ |= kSetViewportFlag;

  if (BlendStates_.size() > 1)
  {
    BlendStates_ = { std::move(BlendStates_.back()) };
  }
  RendererCommandFlags_ |= kSetBlendStateFlag;

  if (DepthStencilStates_.size() > 1)
  {
    DepthStencilStates_ = { std::move(DepthStencilStates_.back()) };
  }
  RendererCommandFlags_ |= kSetDepthStencilStateFlag;

  if (RasterizerStates_.size() > 1)
  {
    RasterizerStates_ = { std::move(RasterizerStates_.back()) };
  }
  RendererCommandFlags_ |= kSetRasterizerStateFlag;

  if (VertexShaders_.size() > 1)
  {
    VertexShaders_ = { std::move(VertexShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetVertexShaderFlag;

  if (HullShaders_.size() > 1)
  {
    HullShaders_ = { std::move(HullShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetHullShaderFlag;

  if (DomainShaders_.size() > 1)
  {
    DomainShaders_ = { std::move(DomainShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetDomainShaderFlag;

  if (GeometryShaders_.size() > 1)
  {
    GeometryShaders_ = { std::move(GeometryShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetGeometryShaderFlag;

  if (PixelShaders_.size() > 1)
  {
    PixelShaders_ = { std::move(PixelShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetPixelShaderFlag;

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  {
    auto& ShaderResources = ShaderResources_[ShaderTypeCount];

    if (ShaderResources.size() > 1)
    {
      ShaderResources = { std::move(ShaderResources.back()) };
    }
    RendererCommandFlags_ |= kSetVertexStageShaderResourceFlag << ShaderTypeCount;
  }

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  {
    auto& Samplers = Samplers_[ShaderTypeCount];

    if (Samplers.size() > 1)
    {
      Samplers = { std::move(Samplers.back()) };
    }
    RendererCommandFlags_ |= kSetVertexStageSamplerFlag << ShaderTypeCount;
  }

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  {
    auto& ConstantBuffers = ConstantBuffers_[ShaderTypeCount];

    if (ConstantBuffers.size() > 1)
    {
      ConstantBuffers = { std::move(ConstantBuffers.back()) };
    }
    RendererCommandFlags_ |= kSetVertexStageConstantBufferFlag << ShaderTypeCount;
  }
}

void RendererCommandList<std::nullptr_t, std::nullptr_t>::SetDrawData(vdl::uint _VertexCount)
{
  PushRendererCommand();

  //  Draw
  {
    RendererCommands_.emplace_back(kDrawFlag, static_cast<vdl::uint>(DrawDatas_.size()));
    DrawDatas_.emplace_back(_VertexCount);
  }
}

//--------------------------------------------------

template<class InstanceData>
void RendererCommandList<vdl::Texture, InstanceData>::Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState, vdl::RasterizerState&& _RasterizerState,
  vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, VertexBuffer&& _VertexBuffer, InstanceBuffer&& _InstanceBuffer)
{
  pBufferManager_ = Engine::Get<IBufferManager>();

  InstanceBuffer_ = std::move(_InstanceBuffer);
  CurrentVertexBuffer_ = std::move(_VertexBuffer);
  InputLayouts_.push_back(CurrentInputLayout_ = std::move(_InputLayout));
  Topologys_.push_back(CurrentTopology_ = std::move(_Topology));
  Scissors_.push_back(CurrentScissor_ = vdl::Scissor());
  Viewports_.push_back(CurrentViewport_ = vdl::Viewport());
  BlendStates_.push_back(CurrentBlendState_ = std::move(_BlendState));
  DepthStencilStates_.push_back(CurrentDepthStencilState_ = std::move(_DepthStencilState));
  RasterizerStates_.push_back(CurrentRasterizerState_ = std::move(_RasterizerState));
  VertexShaders_.push_back(CurrentVertexShader_ = std::move(_VertexShader));
  PixelShaders_.push_back(CurrentPixelShader_ = std::move(_PixelShader));
  HullShaders_.push_back(CurrentHullShader_ = vdl::HullShader());
  DomainShaders_.push_back(CurrentDomainShader_ = vdl::DomainShader());
  GeometryShaders_.push_back(CurrentGeometryShader_ = vdl::GeometryShader());

  for (auto& ShaderResources : ShaderResources_) { ShaderResources.resize(1); }
  for (auto& Samplers : Samplers_) { Samplers.resize(1); }
  for (auto& ConstantBuffers : ConstantBuffers_) { ConstantBuffers.resize(1); }

  Samplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][0].resize(1);
  CurrentSamplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)].resize(1);
  Samplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][0][0] = CurrentSamplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][0] = std::move(_Sampler);

  Reset();
}

template<class InstanceData>
void RendererCommandList<vdl::Texture, InstanceData>::Reset()
{
  RendererCommands_.clear();
  RendererCommandFlags_ = 0;
  DisplayObjectIDs_.clear();
  Instances_.clear();
  DrawDatas_.clear();
  ReservedDisplayObjects_.clear();

  RendererCommandFlags_ |= kSetInstanceBufferFlag;
  VertexBuffers_.clear();
  RendererCommandFlags_ |= kSetVertexBufferFlag;
  IndexBuffers_.clear();
  RendererCommandFlags_ |= kSetIndexBufferFlag;

  if (Topologys_.size() > 1)
  {
    Topologys_ = { std::move(Topologys_.back()) };
  }
  RendererCommandFlags_ |= kSetTopologyFlag;

  if (Scissors_.size() > 1)
  {
    Scissors_ = { std::move(Scissors_.back()) };
  }
  RendererCommandFlags_ |= kSetScissorFlag;

  if (Viewports_.size() > 1)
  {
    Viewports_ = { std::move(Viewports_.back()) };
  }
  RendererCommandFlags_ |= kSetViewportFlag;

  if (BlendStates_.size() > 1)
  {
    BlendStates_ = { std::move(BlendStates_.back()) };
  }
  RendererCommandFlags_ |= kSetBlendStateFlag;

  if (DepthStencilStates_.size() > 1)
  {
    DepthStencilStates_ = { std::move(DepthStencilStates_.back()) };
  }
  RendererCommandFlags_ |= kSetDepthStencilFlag;

  if (RasterizerStates_.size() > 1)
  {
    RasterizerStates_ = { std::move(RasterizerStates_.back()) };
  }
  RendererCommandFlags_ |= kSetRasterizerStateFlag;

  if (VertexShaders_.size() > 1)
  {
    VertexShaders_ = { std::move(VertexShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetVertexShaderFlag;

  if (HullShaders_.size() > 1)
  {
    HullShaders_ = { std::move(HullShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetHullShaderFlag;

  if (DomainShaders_.size() > 1)
  {
    DomainShaders_ = { std::move(DomainShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetDomainShaderFlag;

  if (GeometryShaders_.size() > 1)
  {
    GeometryShaders_ = { std::move(GeometryShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetGeometryShaderFlag;

  if (PixelShaders_.size() > 1)
  {
    PixelShaders_ = { std::move(PixelShaders_.back()) };
  }
  RendererCommandFlags_ |= kSetPixelShaderFlag;

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  {
    auto& ShaderResources = ShaderResources_[ShaderTypeCount];

    if (ShaderResources.size() > 1)
    {
      ShaderResources = { std::move(ShaderResources.back()) };
    }
    RendererCommandFlags_ |= kSetVertexStageShaderResourceFlag << ShaderTypeCount;
  }

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  {
    auto& Samplers = Samplers_[ShaderTypeCount];

    if (Samplers.size() > 1)
    {
      Samplers = { std::move(Samplers.back()) };
    }
    RendererCommandFlags_ |= kSetVertexStageSamplerFlag << ShaderTypeCount;
  }

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  {
    auto& ConstantBuffers = ConstantBuffers_[ShaderTypeCount];

    if (ConstantBuffers.size() > 1)
    {
      ConstantBuffers = { std::move(ConstantBuffers.back()) };
    }
    RendererCommandFlags_ |= kSetVertexStageConstantBufferFlag << ShaderTypeCount;
  }
}

template<class InstanceData>
void RendererCommandList<vdl::Texture, InstanceData>::SetDrawData(const vdl::Texture& _Texture, InstanceData&& _InstanceData)
{
  PushRendererCommand();

  //  Draw
  {
    if (RendererCommandFlags_ & kSetVertexBufferFlag)
    {
      RendererCommands_.emplace_back(kSetVertexBufferFlag, static_cast<vdl::uint>(VertexBuffers_.size()));
      VertexBuffers_.push_back(CurrentVertexBuffer_);
      RendererCommandFlags_ &= | kSetVertexBufferFlag;
    }

    const vdl::ID DiplayObjectID = _DisplayObject.GetID();

    if (ReservedDisplayObjects_.find(DiplayObjectID) == ReservedDisplayObjects_.end())
    {
      ReservedDisplayObjects_.insert(std::make_pair(DiplayObjectID, _DisplayObject));
    }

    RendererCommands_.emplace_back(kDrawFlag, static_cast<vdl::uint>(Instances_.size()));
    DrawDatas_.emplace_back(4);
    DisplayObjectIDs_.push_back(DiplayObjectID);
    Instances_.emplace_back(std::move(_InstanceData));
  }
}
