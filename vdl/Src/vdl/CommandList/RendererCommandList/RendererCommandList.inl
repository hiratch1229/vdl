#include <vdl/Engine.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/ModelManager/IModelManager.hpp>

#include <assert.h>

inline void BaseRendererCommandList::Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState,
  vdl::RasterizerState&& _RasterizerState, vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, InstanceBuffer&& _InstanceBuffer)
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

inline void BaseRendererCommandList::Reset()
{
  RendererCommands_.clear();
  RendererCommandFlags_ = 0;
  DisplayObjectIDs_.clear();
  Instances_.clear();
  DrawDatas_.clear();
  DrawIndexedDatas_.clear();
  ReservedDisplayObjects_.clear();

  //  インスタンスバッファが存在しているならコマンドに書き込んでおく
  if (!InstanceBuffer_.isEmpty())
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetInstanceBufferFlag)), 0);
  }

  if (VertexBuffers_.size() > 1)
  {
    VertexBuffers_ = { std::move(VertexBuffers_.back()) };
  }
  //  頂点バッファが存在しているならフラグを立てる
  if (!CurrentVertexBuffer_.isEmpty())
  {
    RendererCommandFlags_ |= kSetVertexBufferFlag;
  }

  if (IndexBuffers_.size() > 1)
  {
    IndexBuffers_ = { std::move(IndexBuffers_.back()) };
  }
  //  インデックスバッファが存在しているならフラグを立てる
  if (!CurrentIndexBuffer_.isEmpty())
  {
    RendererCommandFlags_ |= kSetIndexBufferFlag;
  }

  if (InputLayouts_.size() > 1)
  {
    InputLayouts_ = { std::move(InputLayouts_.back()) };
  }
  RendererCommandFlags_ |= kSetInputLayoutFlag;

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

#define SetState(StateName)\
if (Current##StateName##_ == _##StateName)\
{\
  return;\
}\
\
RendererCommandFlags_ |= kSet##StateName##Flag;\
Current##StateName##_ = _##StateName;
inline void BaseRendererCommandList::SetVertexBuffer(const VertexBuffer& _VertexBuffer) { SetState(VertexBuffer) }

inline void BaseRendererCommandList::SetIndexBuffer(const IndexBuffer& _IndexBuffer) { SetState(IndexBuffer) }

inline void BaseRendererCommandList::SetInputLayout(vdl::InputLayoutType _InputLayout) { SetState(InputLayout) }

inline void BaseRendererCommandList::SetTopology(vdl::TopologyType _Topology) { SetState(Topology) }

inline void BaseRendererCommandList::SetScissor(const vdl::Scissor& _Scissor) { SetState(Scissor) }

inline void BaseRendererCommandList::SetViewport(const vdl::Viewport& _Viewport) { SetState(Viewport) }

inline void BaseRendererCommandList::SetBlendState(const vdl::BlendState& _BlendState) { SetState(BlendState) }

inline void BaseRendererCommandList::SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState) { SetState(DepthStencilState) }

inline void BaseRendererCommandList::SetRasterizerState(const vdl::RasterizerState& _RasterizerState) { SetState(RasterizerState) }

inline void BaseRendererCommandList::SetVertexShader(const vdl::VertexShader& _VertexShader) { SetState(VertexShader) }

inline void BaseRendererCommandList::SetHullShader(const vdl::HullShader& _HullShader) { SetState(HullShader) }

inline void BaseRendererCommandList::SetDomainShader(const vdl::DomainShader& _DomainShader) { SetState(DomainShader) }

inline void BaseRendererCommandList::SetGeometryShader(const vdl::GeometryShader& _GeometryShader) { SetState(GeometryShader) }

inline void BaseRendererCommandList::SetPixelShader(const vdl::PixelShader& _PixelShader) { SetState(PixelShader) }
#undef SetState

#define SetShaderState(StateName)\
constexpr RendererCommandFlags kSet##StateName##Flag = kSetVertexStage##StateName##Flag << static_cast<vdl::uint>(Type);\
\
StateName##s& Current##StateName##s = Current##StateName##s_[static_cast<vdl::uint>(Type)];\
\
if (const vdl::uint RequiredSize = _StartSlot + _##StateName##Num;\
Current##StateName##s.size() < RequiredSize)\
{\
  Current##StateName##s.resize(static_cast<size_t>(RequiredSize));\
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
inline void BaseRendererCommandList::SetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]) { SetShaderState(ShaderResource) }

template<ShaderType Type>
inline void BaseRendererCommandList::SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]) { SetShaderState(Sampler) }

template<ShaderType Type>
inline void BaseRendererCommandList::SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]) { SetShaderState(ConstantBuffer) }
#undef SetShaderState

template<ShaderType Type>
inline void BaseRendererCommandList::UpdateAndSetConstantBuffer()
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
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetConstantBufferFlag)), static_cast<vdl::uint>(ConstantBuffers.size()));
    PreviousConstantBuffers = CurrentConstantBuffers;
    ConstantBuffers.emplace_back(std::move(TempConstantBuffers));
    RendererCommandFlags_ &= ~kSetConstantBufferFlag;
  }
}

template<ShaderType Type>
inline void BaseRendererCommandList::SetShaderObjects()
{
  //  SetShaderResource
  {
    constexpr RendererCommandFlags kSetShaderResourceFlag = kSetVertexStageShaderResourceFlag << static_cast<vdl::uint>(Type);
    if (RendererCommandFlags_ & kSetShaderResourceFlag)
    {
      auto& ShaderResources = ShaderResources_[static_cast<vdl::uint>(Type)];

      RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetShaderResourceFlag)), static_cast<vdl::uint>(ShaderResources.size()));
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

      RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetSamplerFlag)), static_cast<vdl::uint>(Sampler.size()));
      Sampler.push_back(CurrentSamplers_[static_cast<vdl::uint>(Type)]);

      RendererCommandFlags_ &= ~kSetSamplerFlag;
    }
  }

  UpdateAndSetConstantBuffer<Type>();
}

inline void BaseRendererCommandList::PushRendererCommand()
{
  if (RendererCommandFlags_ & kSetInputLayoutFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetInputLayoutFlag)), static_cast<vdl::uint>(InputLayouts_.size()));
    InputLayouts_.push_back(CurrentInputLayout_);

    RendererCommandFlags_ &= ~kSetInputLayoutFlag;
  }

  if (RendererCommandFlags_ & kSetVertexBufferFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetVertexBufferFlag)), static_cast<vdl::uint>(VertexBuffers_.size()));
    VertexBuffers_.push_back(CurrentVertexBuffer_);

    RendererCommandFlags_ &= ~kSetVertexBufferFlag;
  }

  if (RendererCommandFlags_ & kSetIndexBufferFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetIndexBufferFlag)), static_cast<vdl::uint>(IndexBuffers_.size()));
    IndexBuffers_.push_back(CurrentIndexBuffer_);

    RendererCommandFlags_ &= ~kSetIndexBufferFlag;
  }

  if (RendererCommandFlags_ & kSetTopologyFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetTopologyFlag)), static_cast<vdl::uint>(Topologys_.size()));
    Topologys_.push_back(CurrentTopology_);

    RendererCommandFlags_ &= ~kSetTopologyFlag;
  }

  if (RendererCommandFlags_ & kSetScissorFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetScissorFlag)), static_cast<vdl::uint>(Scissors_.size()));
    Scissors_.push_back(CurrentScissor_);

    RendererCommandFlags_ &= ~kSetScissorFlag;
  }

  if (RendererCommandFlags_ & kSetViewportFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetViewportFlag)), static_cast<vdl::uint>(Viewports_.size()));
    Viewports_.push_back(CurrentViewport_);

    RendererCommandFlags_ &= ~kSetViewportFlag;
  }

  if (RendererCommandFlags_ & kSetBlendStateFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetBlendStateFlag)), static_cast<vdl::uint>(BlendStates_.size()));
    BlendStates_.push_back(CurrentBlendState_);

    RendererCommandFlags_ &= ~kSetBlendStateFlag;
  }

  if (RendererCommandFlags_ & kSetDepthStencilStateFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetDepthStencilStateFlag)), static_cast<vdl::uint>(DepthStencilStates_.size()));
    DepthStencilStates_.push_back(CurrentDepthStencilState_);

    RendererCommandFlags_ &= ~kSetDepthStencilStateFlag;
  }

  if (RendererCommandFlags_ & kSetRasterizerStateFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetRasterizerStateFlag)), static_cast<vdl::uint>(RasterizerStates_.size()));
    RasterizerStates_.push_back(CurrentRasterizerState_);

    RendererCommandFlags_ &= ~kSetRasterizerStateFlag;
  }

  if (RendererCommandFlags_ & kSetVertexShaderFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetVertexShaderFlag)), static_cast<vdl::uint>(VertexShaders_.size()));
    VertexShaders_.push_back(CurrentVertexShader_);

    RendererCommandFlags_ &= ~kSetVertexShaderFlag;
  }

  if (RendererCommandFlags_ & kSetHullShaderFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetHullShaderFlag)), static_cast<vdl::uint>(HullShaders_.size()));
    HullShaders_.push_back(CurrentHullShader_);

    RendererCommandFlags_ &= ~kSetHullShaderFlag;
  }

  if (RendererCommandFlags_ & kSetDomainShaderFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetDomainShaderFlag)), static_cast<vdl::uint>(DomainShaders_.size()));
    DomainShaders_.push_back(CurrentDomainShader_);

    RendererCommandFlags_ &= ~kSetDomainShaderFlag;
  }

  if (RendererCommandFlags_ & kSetGeometryShaderFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetGeometryShaderFlag)), static_cast<vdl::uint>(GeometryShaders_.size()));
    GeometryShaders_.push_back(CurrentGeometryShader_);

    RendererCommandFlags_ &= ~kSetGeometryShaderFlag;
  }

  if (RendererCommandFlags_ & kSetPixelShaderFlag)
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kSetPixelShaderFlag)), static_cast<vdl::uint>(PixelShaders_.size()));
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

inline void BaseRendererCommandList::Sort()
{
  //  ソートが不可能な時は終了
  if (!CurrentDepthStencilState_.DepthEnable)
  {
    return;
  }

  //  2つのDrawが混じっているCommmandListは未対応
  assert(DrawDatas_.empty() || DrawIndexedDatas_.empty());

  using Iterator = RendererCommands::iterator;

  Iterator StartDrawItr;
  {
    //  未ソートのDrawCommandの開始点を検索
    Iterator EndDrawItr = RendererCommands_.end() - 1;
    for (Iterator Itr = EndDrawItr, Start = RendererCommands_.begin(); Start <= Itr; --Itr)
    {
      //  DrawCommandでなくなるまで検索
      if (Itr->first != RendererCommandFlag::eDraw && Itr->first != RendererCommandFlag::eDrawIndexed)
      {
        StartDrawItr = Itr + 1;
        break;
      }
    }

    //  参照するID順にソートする
    std::sort(StartDrawItr, EndDrawItr,
      [&](const RendererCommandPair& _Command0, const RendererCommandPair& _Command1)
    {
      return DisplayObjectIDs_[_Command0.second] < DisplayObjectIDs_[_Command1.second];
    });
  }

  Iterator NextItr;
  //  RendererCommandFlag::eDraw
  if (!DrawDatas_.empty())
  {
    for (Iterator Itr = StartDrawItr; Itr < RendererCommands_.end() - 1;)
    {
      NextItr = Itr + 1;
      //  違うオブジェクトのDrawCommandの場合は次へ
      if (DisplayObjectIDs_[Itr->second] != DisplayObjectIDs_[NextItr->second])
      {
        ++Itr;
        continue;
      }

      //  DrawCommandを1つに統合する(Instancing)
      {
        assert(DrawDatas_[Itr->second].FirstVertex == DrawDatas_[NextItr->second].FirstVertex
          && DrawDatas_[Itr->second].VertexCount == DrawDatas_[NextItr->second].VertexCount);

        DrawDatas_[Itr->second].InstanceCount += DrawDatas_[NextItr->second].InstanceCount;
        DrawDatas_[NextItr->second].InstanceCount = 0;

        Instances_[Itr->second].insert(Instances_[Itr->second].end(), Instances_[NextItr->second].cbegin(), Instances_[NextItr->second].cend());

        Itr = RendererCommands_.erase(NextItr);
      }
    }
  }
  //  RendererCommandFlag::eDrawIndexed
  else
  {
    for (Iterator Itr = StartDrawItr; Itr < RendererCommands_.end() - 1;)
    {
      NextItr = Itr + 1;
      //  違うオブジェクトのDrawCommandの場合は次へ
      if (DisplayObjectIDs_[Itr->second] != DisplayObjectIDs_[NextItr->second])
      {
        ++Itr;
        continue;
      }

      //  DrawCommandを1つに統合する(Instancing)
      {
        assert(DrawIndexedDatas_[Itr->second].FirstIndex == DrawIndexedDatas_[NextItr->second].FirstIndex
          && DrawIndexedDatas_[Itr->second].IndexCount == DrawIndexedDatas_[NextItr->second].IndexCount
          && DrawIndexedDatas_[Itr->second].VertexOffset == DrawIndexedDatas_[NextItr->second].VertexOffset);

        DrawIndexedDatas_[Itr->second].InstanceCount += DrawIndexedDatas_[NextItr->second].InstanceCount;
        DrawIndexedDatas_[NextItr->second].InstanceCount = 0;

        Instances_[Itr->second].insert(Instances_[Itr->second].end(), Instances_[NextItr->second].cbegin(), Instances_[NextItr->second].cend());

        Itr = RendererCommands_.erase(NextItr);
      }
    }
  }
}

//--------------------------------------------------

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState,
  vdl::RasterizerState&& _RasterizerState, vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, InstanceBuffer&& _InstanceBuffer)
{
  pModelManager_ = Engine::Get<IModelManager>();

  BaseRendererCommandList::Initialize(_InputLayout, _Topology, std::move(_BlendState), std::move(_DepthStencilState), std::move(_RasterizerState), std::move(_Sampler), std::move(_VertexShader), std::move(_PixelShader), std::move(_InstanceBuffer));
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::SetDrawData(const DisplayObject& _DisplayObject, InstanceData&& _InstanceData)
{
  const vdl::ID DiplayObjectID = _DisplayObject.GetID();
  const Mesh* pMesh = pModelManager_->GetMesh(DiplayObjectID);

  //  必要ステートの設定
  {
    BaseRendererCommandList::SetVertexBuffer(pMesh->VertexBuffer);
    BaseRendererCommandList::SetIndexBuffer(pMesh->IndexBuffer);

    vdl::ShaderResource ShaderResources[2] = { pMesh->Material.Diffuse, pMesh->Material.NormalMap };
    BaseRendererCommandList::SetShaderResources<ShaderType::ePixelShader>(0, 2, ShaderResources);
  }

  //  現在のステートのコマンドを発行
  BaseRendererCommandList::PushRendererCommand();

  //  Draw
  {
    if (ReservedDisplayObjects_.find(DiplayObjectID) == ReservedDisplayObjects_.end())
    {
      ReservedDisplayObjects_.insert(std::make_pair(DiplayObjectID, _DisplayObject));
    }

    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kDrawIndexedFlag)), static_cast<vdl::uint>(DrawIndexedDatas_.size()));
    DrawIndexedDatas_.emplace_back(pMesh->IndexCount, 1, pMesh->IndexStart, 0, 0);
    DisplayObjectIDs_.push_back(DiplayObjectID);

    auto& InstanceDatas = Instances_.emplace_back(kInstanceSize);
    ::memmove(InstanceDatas.data(), &_InstanceData, kInstanceSize);

    Sort();
  }
}

//--------------------------------------------------

template<class InstanceData>
inline void RendererCommandList<vdl::Texture, InstanceData>::Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState, vdl::RasterizerState&& _RasterizerState,
  vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, VertexBuffer&& _VertexBuffer, InstanceBuffer&& _InstanceBuffer)
{
  VertexBuffers_.push_back(CurrentVertexBuffer_ = std::move(_VertexBuffer));
  BaseRendererCommandList::Initialize(_InputLayout, _Topology, std::move(_BlendState), std::move(_DepthStencilState), std::move(_RasterizerState), std::move(_Sampler), std::move(_VertexShader), std::move(_PixelShader), std::move(_InstanceBuffer));
}

template<class InstanceData>
inline void RendererCommandList<vdl::Texture, InstanceData>::SetDrawData(const vdl::Texture& _Texture, InstanceData&& _InstanceData)
{
  //  必要ステートの設定
  BaseRendererCommandList::SetShaderResources<ShaderType::ePixelShader>(0, 1, &vdl::ShaderResource(_Texture));

  //  現在のステートのコマンドを発行
  BaseRendererCommandList::PushRendererCommand();

  //  Draw
  {
    const vdl::ID DiplayObjectID = _Texture.GetID();

    if (ReservedDisplayObjects_.find(DiplayObjectID) == ReservedDisplayObjects_.end())
    {
      ReservedDisplayObjects_.insert(std::make_pair(DiplayObjectID, _Texture));
    }

    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kDrawFlag)), static_cast<vdl::uint>(DrawDatas_.size()));
    DrawDatas_.emplace_back(4, 1, 0, 0);
    DisplayObjectIDs_.push_back(DiplayObjectID);

    auto& InstanceDatas = Instances_.emplace_back(kInstanceSize);
    ::memmove(InstanceDatas.data(), &_InstanceData, kInstanceSize);

    Sort();
  }
}

//--------------------------------------------------

inline void RendererCommandList<vdl::Texture, std::nullptr_t>::Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState,
  vdl::RasterizerState&& _RasterizerState, vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader)
{
  BaseRendererCommandList::Initialize(_InputLayout, _Topology, std::move(_BlendState), std::move(_DepthStencilState), std::move(_RasterizerState), std::move(_Sampler), std::move(_VertexShader), std::move(_PixelShader), InstanceBuffer());
}

inline void RendererCommandList<vdl::Texture, std::nullptr_t>::SetDrawData(const vdl::Texture& _Texture, DrawIndexedData&& _DrawIndexedData)
{
  //  必要ステートの設定
  BaseRendererCommandList::SetShaderResources<ShaderType::ePixelShader>(0, 1, &vdl::ShaderResource(_Texture));

  //  現在のステートのコマンドを発行
  BaseRendererCommandList::PushRendererCommand();

  //  Draw
  {
    const vdl::ID DiplayObjectID = _Texture.GetID();

    if (ReservedDisplayObjects_.find(DiplayObjectID) == ReservedDisplayObjects_.end())
    {
      ReservedDisplayObjects_.insert(std::make_pair(DiplayObjectID, _Texture));
    }

    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kDrawIndexedFlag)), static_cast<vdl::uint>(DrawIndexedDatas_.size()));
    DrawIndexedDatas_.emplace_back(std::move(_DrawIndexedData));
    DisplayObjectIDs_.push_back(DiplayObjectID);
  }
}

//--------------------------------------------------

inline void RendererCommandList<std::nullptr_t, std::nullptr_t>::Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState, vdl::RasterizerState&& _RasterizerState,
  vdl::Sampler&& _Sampler)
{
  BaseRendererCommandList::Initialize(_InputLayout, _Topology, std::move(_BlendState), std::move(_DepthStencilState), std::move(_RasterizerState), std::move(_Sampler), vdl::VertexShader(), vdl::PixelShader(), InstanceBuffer());
}

inline void RendererCommandList<std::nullptr_t, std::nullptr_t>::SetDrawData(DrawData&& _DrawData)
{
  //  現在のステートのコマンドを発行
  BaseRendererCommandList::PushRendererCommand();

  //  Draw
  {
    RendererCommands_.emplace_back(static_cast<RendererCommandFlag>(static_cast<vdl::uint>(kDrawFlag)), static_cast<vdl::uint>(DrawDatas_.size()));
    DrawDatas_.emplace_back(std::move(_DrawData));
  }
}
