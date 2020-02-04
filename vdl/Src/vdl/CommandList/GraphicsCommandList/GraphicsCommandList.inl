#include <vdl/Engine.hpp>
#include <vdl/System/ISystem.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/ModelManager/IModelManager.hpp>

#include <assert.h>

inline void BaseGraphicsCommandList::Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState,
  vdl::RasterizerState&& _RasterizerState, vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, InstanceBuffer&& _InstanceBuffer)
{
  pBufferManager_ = Engine::Get<IBufferManager>();
  pThreadPool_ = Engine::Get<ISystem>()->GetThreadPool();

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

  ResetFunc_ = [&]()
  {
    {
      std::lock_guard Lock(Mutex_);
      GraphicsCommandFlags_ = 0xFFFFFFFF;

      //  インスタンスバッファが存在していないならフラグを外す
      if (InstanceBuffer_.isEmpty())
      {
        GraphicsCommandFlags_ &= ~kSetInstanceBufferFlag;
      }

      //  頂点バッファが存在していないならフラグを外す
      if (CurrentVertexBuffer_.isEmpty())
      {
        GraphicsCommandFlags_ &= ~kSetVertexBufferFlag;
      }

      //  インデックスバッファが存在していないならフラグを外す
      if (CurrentIndexBuffer_.isEmpty())
      {
        GraphicsCommandFlags_ &= ~kSetIndexBufferFlag;
      }
    }

    std::lock_guard ResetLock(ResetMutex_);

    GraphicsCommands_.clear();
    DisplayObjectIDs_.clear();
    Instances_.clear();
    DrawDatas_.clear();
    DrawIndexedDatas_.clear();
    ReservedDisplayObjects_.clear();

    if (VertexBuffers_.size() > 1)
    {
      VertexBuffers_ = { std::move(VertexBuffers_.back()) };
    }

    if (IndexBuffers_.size() > 1)
    {
      IndexBuffers_ = { std::move(IndexBuffers_.back()) };
    }

    if (InputLayouts_.size() > 1)
    {
      InputLayouts_ = { std::move(InputLayouts_.back()) };
    }

    if (Topologys_.size() > 1)
    {
      Topologys_ = { std::move(Topologys_.back()) };
    }

    if (Scissors_.size() > 1)
    {
      Scissors_ = { std::move(Scissors_.back()) };
    }

    if (Viewports_.size() > 1)
    {
      Viewports_ = { std::move(Viewports_.back()) };
    }

    if (BlendStates_.size() > 1)
    {
      BlendStates_ = { std::move(BlendStates_.back()) };
    }

    if (DepthStencilStates_.size() > 1)
    {
      DepthStencilStates_ = { std::move(DepthStencilStates_.back()) };
    }

    if (RasterizerStates_.size() > 1)
    {
      RasterizerStates_ = { std::move(RasterizerStates_.back()) };
    }

    if (VertexShaders_.size() > 1)
    {
      VertexShaders_ = { std::move(VertexShaders_.back()) };
    }

    if (HullShaders_.size() > 1)
    {
      HullShaders_ = { std::move(HullShaders_.back()) };
    }

    if (DomainShaders_.size() > 1)
    {
      DomainShaders_ = { std::move(DomainShaders_.back()) };
    }

    if (GeometryShaders_.size() > 1)
    {
      GeometryShaders_ = { std::move(GeometryShaders_.back()) };
    }

    if (PixelShaders_.size() > 1)
    {
      PixelShaders_ = { std::move(PixelShaders_.back()) };
    }

    for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
    {
      auto& ShaderResources = ShaderResources_[ShaderTypeCount];

      if (ShaderResources.size() > 1)
      {
        ShaderResources = { std::move(ShaderResources.back()) };
      }
    }

    for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
    {
      auto& Samplers = Samplers_[ShaderTypeCount];

      if (Samplers.size() > 1)
      {
        Samplers = { std::move(Samplers.back()) };
      }
    }

    for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
    {
      auto& ConstantBuffers = ConstantBuffers_[ShaderTypeCount];

      if (ConstantBuffers.size() > 1)
      {
        ConstantBuffers = { std::move(ConstantBuffers.back()) };
      }
    }

    isReady_ = true;
    ConditionVariable_.notify_one();
  };

  Reset();
}

inline void BaseGraphicsCommandList::Reset()
{
  isReady_ = false;

  pThreadPool_->Enqueue(ResetFunc_);
  //ResetFunc_();

  //GraphicsCommands_.clear();
  //GraphicsCommandFlags_ = 0;
  //DisplayObjectIDs_.clear();
  //Instances_.clear();
  //DrawDatas_.clear();
  //DrawIndexedDatas_.clear();
  //ReservedDisplayObjects_.clear();
  //
  ////  インスタンスバッファが存在しているならフラグを立てる
  //if (!InstanceBuffer_.isEmpty())
  //{
  //  GraphicsCommandFlags_ |= kSetInstanceBufferFlag;
  //}
  //
  //if (VertexBuffers_.size() > 1)
  //{
  //  VertexBuffers_ = { std::move(VertexBuffers_.back()) };
  //}
  ////  頂点バッファが存在しているならフラグを立てる
  //if (!CurrentVertexBuffer_.isEmpty())
  //{
  //  GraphicsCommandFlags_ |= kSetVertexBufferFlag;
  //}
  //
  //if (IndexBuffers_.size() > 1)
  //{
  //  IndexBuffers_ = { std::move(IndexBuffers_.back()) };
  //}
  ////  インデックスバッファが存在しているならフラグを立てる
  //if (!CurrentIndexBuffer_.isEmpty())
  //{
  //  GraphicsCommandFlags_ |= kSetIndexBufferFlag;
  //}
  //
  //if (InputLayouts_.size() > 1)
  //{
  //  InputLayouts_ = { std::move(InputLayouts_.back()) };
  //}
  //GraphicsCommandFlags_ |= kSetInputLayoutFlag;
  //
  //if (Topologys_.size() > 1)
  //{
  //  Topologys_ = { std::move(Topologys_.back()) };
  //}
  //GraphicsCommandFlags_ |= kSetTopologyFlag;
  //
  //if (Scissors_.size() > 1)
  //{
  //  Scissors_ = { std::move(Scissors_.back()) };
  //}
  //GraphicsCommandFlags_ |= kSetScissorFlag;
  //
  //if (Viewports_.size() > 1)
  //{
  //  Viewports_ = { std::move(Viewports_.back()) };
  //}
  //GraphicsCommandFlags_ |= kSetViewportFlag;
  //
  //if (BlendStates_.size() > 1)
  //{
  //  BlendStates_ = { std::move(BlendStates_.back()) };
  //}
  //GraphicsCommandFlags_ |= kSetBlendStateFlag;
  //
  //if (DepthStencilStates_.size() > 1)
  //{
  //  DepthStencilStates_ = { std::move(DepthStencilStates_.back()) };
  //}
  //GraphicsCommandFlags_ |= kSetDepthStencilStateFlag;
  //
  //if (RasterizerStates_.size() > 1)
  //{
  //  RasterizerStates_ = { std::move(RasterizerStates_.back()) };
  //}
  //GraphicsCommandFlags_ |= kSetRasterizerStateFlag;
  //
  //if (VertexShaders_.size() > 1)
  //{
  //  VertexShaders_ = { std::move(VertexShaders_.back()) };
  //}
  //GraphicsCommandFlags_ |= kSetVertexShaderFlag;
  //
  //if (HullShaders_.size() > 1)
  //{
  //  HullShaders_ = { std::move(HullShaders_.back()) };
  //}
  //GraphicsCommandFlags_ |= kSetHullShaderFlag;
  //
  //if (DomainShaders_.size() > 1)
  //{
  //  DomainShaders_ = { std::move(DomainShaders_.back()) };
  //}
  //GraphicsCommandFlags_ |= kSetDomainShaderFlag;
  //
  //if (GeometryShaders_.size() > 1)
  //{
  //  GeometryShaders_ = { std::move(GeometryShaders_.back()) };
  //}
  //GraphicsCommandFlags_ |= kSetGeometryShaderFlag;
  //
  //if (PixelShaders_.size() > 1)
  //{
  //  PixelShaders_ = { std::move(PixelShaders_.back()) };
  //}
  //GraphicsCommandFlags_ |= kSetPixelShaderFlag;
  //
  //for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  //{
  //  auto& ShaderResources = ShaderResources_[ShaderTypeCount];
  //
  //  if (ShaderResources.size() > 1)
  //  {
  //    ShaderResources = { std::move(ShaderResources.back()) };
  //  }
  //  GraphicsCommandFlags_ |= kSetVertexStageShaderResourceFlag << ShaderTypeCount;
  //}
  //
  //for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  //{
  //  auto& Samplers = Samplers_[ShaderTypeCount];
  //
  //  if (Samplers.size() > 1)
  //  {
  //    Samplers = { std::move(Samplers.back()) };
  //  }
  //  GraphicsCommandFlags_ |= kSetVertexStageSamplerFlag << ShaderTypeCount;
  //}
  //
  //for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  //{
  //  auto& ConstantBuffers = ConstantBuffers_[ShaderTypeCount];
  //
  //  if (ConstantBuffers.size() > 1)
  //  {
  //    ConstantBuffers = { std::move(ConstantBuffers.back()) };
  //  }
  //  GraphicsCommandFlags_ |= kSetVertexStageConstantBufferFlag << ShaderTypeCount;
  //}
}

#define SetState(StateName)\
std::lock_guard Lock(Mutex_);\
\
if (Current##StateName##_ == _##StateName)\
{\
  return;\
}\
\
GraphicsCommandFlags_ |= kSet##StateName##Flag;\
Current##StateName##_ = _##StateName;
inline void BaseGraphicsCommandList::SetVertexBuffer(const VertexBuffer& _VertexBuffer) { SetState(VertexBuffer) }

inline void BaseGraphicsCommandList::SetIndexBuffer(const IndexBuffer& _IndexBuffer) { SetState(IndexBuffer) }

inline void BaseGraphicsCommandList::SetInputLayout(vdl::InputLayoutType _InputLayout) { SetState(InputLayout) }

inline void BaseGraphicsCommandList::SetTopology(vdl::TopologyType _Topology) { SetState(Topology) }

inline void BaseGraphicsCommandList::SetScissor(const vdl::Scissor& _Scissor) { SetState(Scissor) }

inline void BaseGraphicsCommandList::SetViewport(const vdl::Viewport& _Viewport) { SetState(Viewport) }

inline void BaseGraphicsCommandList::SetBlendState(const vdl::BlendState& _BlendState) { SetState(BlendState) }

inline void BaseGraphicsCommandList::SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState) { SetState(DepthStencilState) }

inline void BaseGraphicsCommandList::SetRasterizerState(const vdl::RasterizerState& _RasterizerState) { SetState(RasterizerState) }

inline void BaseGraphicsCommandList::SetVertexShader(const vdl::VertexShader& _VertexShader) { SetState(VertexShader) }

inline void BaseGraphicsCommandList::SetHullShader(const vdl::HullShader& _HullShader) { SetState(HullShader) }

inline void BaseGraphicsCommandList::SetDomainShader(const vdl::DomainShader& _DomainShader) { SetState(DomainShader) }

inline void BaseGraphicsCommandList::SetGeometryShader(const vdl::GeometryShader& _GeometryShader) { SetState(GeometryShader) }

inline void BaseGraphicsCommandList::SetPixelShader(const vdl::PixelShader& _PixelShader) { SetState(PixelShader) }
#undef SetState

#define SetShaderState(StateName)\
std::lock_guard Lock(Mutex_);\
\
constexpr GraphicsCommandFlags kSet##StateName##Flag = kSetVertexStage##StateName##Flag << static_cast<vdl::uint>(Type);\
\
StateName##s& Current##StateName##s = Current##StateName##s_[static_cast<vdl::uint>(Type)];\
\
if (const vdl::uint RequiredSize = _StartSlot + _##StateName##Num;\
  Current##StateName##s.size() < RequiredSize)\
{\
  Current##StateName##s.resize(static_cast<size_t>(RequiredSize));\
  GraphicsCommandFlags_ |= kSet##StateName##Flag;\
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
  GraphicsCommandFlags_ |= kSet##StateName##Flag;\
  Current##StateName = StateName;\
}
template<ShaderType Type>
inline void BaseGraphicsCommandList::SetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]) { SetShaderState(ShaderResource) }

template<ShaderType Type>
inline void BaseGraphicsCommandList::SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]) { SetShaderState(Sampler) }

template<ShaderType Type>
inline void BaseGraphicsCommandList::SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]) { SetShaderState(ConstantBuffer) }
#undef SetShaderState

template<ShaderType Type>
inline void BaseGraphicsCommandList::UpdateAndSetConstantBuffer()
{
  constexpr GraphicsCommandFlags kSetConstantBufferFlag = kSetVertexStageConstantBufferFlag << static_cast<vdl::uint>(Type);

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
      GraphicsCommandFlags_ |= kSetConstantBufferFlag;

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
      GraphicsCommandFlags_ |= kSetConstantBufferFlag;
    }
  }

  if (GraphicsCommandFlags_ & kSetConstantBufferFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetConstantBufferFlag)), static_cast<vdl::uint>(ConstantBuffers.size()));
    PreviousConstantBuffers = CurrentConstantBuffers;
    ConstantBuffers.emplace_back(std::move(TempConstantBuffers));
    GraphicsCommandFlags_ &= ~kSetConstantBufferFlag;
  }
}

template<ShaderType Type>
inline void BaseGraphicsCommandList::SetShaderObjects()
{
  //  SetShaderResource
  {
    constexpr GraphicsCommandFlags kSetShaderResourceFlag = kSetVertexStageShaderResourceFlag << static_cast<vdl::uint>(Type);
    if (GraphicsCommandFlags_ & kSetShaderResourceFlag)
    {
      auto& ShaderResources = ShaderResources_[static_cast<vdl::uint>(Type)];

      GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetShaderResourceFlag)), static_cast<vdl::uint>(ShaderResources.size()));
      ShaderResources.push_back(CurrentShaderResources_[static_cast<vdl::uint>(Type)]);

      GraphicsCommandFlags_ &= ~kSetShaderResourceFlag;
    }
  }

  //  SetSampler
  {
    constexpr GraphicsCommandFlags kSetSamplerFlag = kSetVertexStageSamplerFlag << static_cast<vdl::uint>(Type);
    if (GraphicsCommandFlags_ & kSetSamplerFlag)
    {
      auto& Sampler = Samplers_[static_cast<vdl::uint>(Type)];

      GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetSamplerFlag)), static_cast<vdl::uint>(Sampler.size()));
      Sampler.push_back(CurrentSamplers_[static_cast<vdl::uint>(Type)]);

      GraphicsCommandFlags_ &= ~kSetSamplerFlag;
    }
  }

  UpdateAndSetConstantBuffer<Type>();
}

inline void BaseGraphicsCommandList::PushGraphicsCommand()
{
  if (GraphicsCommandFlags_ & kSetInputLayoutFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetInputLayoutFlag)), static_cast<vdl::uint>(InputLayouts_.size()));
    InputLayouts_.push_back(CurrentInputLayout_);

    GraphicsCommandFlags_ &= ~kSetInputLayoutFlag;
  }

  if (GraphicsCommandFlags_ & kSetVertexBufferFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetVertexBufferFlag)), static_cast<vdl::uint>(VertexBuffers_.size()));
    VertexBuffers_.push_back(CurrentVertexBuffer_);

    GraphicsCommandFlags_ &= ~kSetVertexBufferFlag;
  }

  if (GraphicsCommandFlags_ & kSetInstanceBufferFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetInstanceBufferFlag)), 0);
    GraphicsCommandFlags_ &= ~kSetInstanceBufferFlag;
  }

  if (GraphicsCommandFlags_ & kSetIndexBufferFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetIndexBufferFlag)), static_cast<vdl::uint>(IndexBuffers_.size()));
    IndexBuffers_.push_back(CurrentIndexBuffer_);

    GraphicsCommandFlags_ &= ~kSetIndexBufferFlag;
  }

  if (GraphicsCommandFlags_ & kSetTopologyFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetTopologyFlag)), static_cast<vdl::uint>(Topologys_.size()));
    Topologys_.push_back(CurrentTopology_);

    GraphicsCommandFlags_ &= ~kSetTopologyFlag;
  }

  if (GraphicsCommandFlags_ & kSetBlendStateFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetBlendStateFlag)), static_cast<vdl::uint>(BlendStates_.size()));
    BlendStates_.push_back(CurrentBlendState_);

    GraphicsCommandFlags_ &= ~kSetBlendStateFlag;
  }

  if (GraphicsCommandFlags_ & kSetDepthStencilStateFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetDepthStencilStateFlag)), static_cast<vdl::uint>(DepthStencilStates_.size()));
    DepthStencilStates_.push_back(CurrentDepthStencilState_);

    GraphicsCommandFlags_ &= ~kSetDepthStencilStateFlag;
  }

  if (GraphicsCommandFlags_ & kSetRasterizerStateFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetRasterizerStateFlag)), static_cast<vdl::uint>(RasterizerStates_.size()));
    RasterizerStates_.push_back(CurrentRasterizerState_);

    GraphicsCommandFlags_ &= ~kSetRasterizerStateFlag;
  }

  if (GraphicsCommandFlags_ & kSetViewportFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetViewportFlag)), static_cast<vdl::uint>(Viewports_.size()));
    Viewports_.push_back(CurrentViewport_);

    GraphicsCommandFlags_ &= ~kSetViewportFlag;
  }

  if (GraphicsCommandFlags_ & kSetScissorFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetScissorFlag)), static_cast<vdl::uint>(Scissors_.size()));
    Scissors_.push_back(CurrentScissor_);

    GraphicsCommandFlags_ &= ~kSetScissorFlag;
  }

  if (GraphicsCommandFlags_ & kSetVertexShaderFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetVertexShaderFlag)), static_cast<vdl::uint>(VertexShaders_.size()));
    VertexShaders_.push_back(CurrentVertexShader_);

    GraphicsCommandFlags_ &= ~kSetVertexShaderFlag;
  }

  if (GraphicsCommandFlags_ & kSetHullShaderFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetHullShaderFlag)), static_cast<vdl::uint>(HullShaders_.size()));
    HullShaders_.push_back(CurrentHullShader_);

    GraphicsCommandFlags_ &= ~kSetHullShaderFlag;
  }

  if (GraphicsCommandFlags_ & kSetDomainShaderFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetDomainShaderFlag)), static_cast<vdl::uint>(DomainShaders_.size()));
    DomainShaders_.push_back(CurrentDomainShader_);

    GraphicsCommandFlags_ &= ~kSetDomainShaderFlag;
  }

  if (GraphicsCommandFlags_ & kSetGeometryShaderFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetGeometryShaderFlag)), static_cast<vdl::uint>(GeometryShaders_.size()));
    GeometryShaders_.push_back(CurrentGeometryShader_);

    GraphicsCommandFlags_ &= ~kSetGeometryShaderFlag;
  }

  if (GraphicsCommandFlags_ & kSetPixelShaderFlag)
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kSetPixelShaderFlag)), static_cast<vdl::uint>(PixelShaders_.size()));
    PixelShaders_.push_back(CurrentPixelShader_);

    GraphicsCommandFlags_ &= ~kSetPixelShaderFlag;
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

inline void BaseGraphicsCommandList::Sort()
{
  //  インスタンシングが無効なリストのソートは不要
  assert(!Instances_.empty());

  //  ソートが不可能な時は終了
  if (!CurrentDepthStencilState_.DepthEnable)
  {
    return;
  }

  //  2つのDrawが混じっているCommmandListは未対応
  assert(DrawDatas_.empty() || DrawIndexedDatas_.empty());

  using Iterator = GraphicsCommands::iterator;

  Iterator StartDrawItr;
  {
    //  未ソートのDrawCommandの開始点を検索
    Iterator EndDrawItr = GraphicsCommands_.end() - 1;
    for (Iterator Itr = EndDrawItr, Start = GraphicsCommands_.begin(); Start <= Itr; --Itr)
    {
      //  DrawCommandでなくなるまで検索
      if (Itr->first != GraphicsCommandFlag::eDraw && Itr->first != GraphicsCommandFlag::eDrawIndexed)
      {
        StartDrawItr = Itr + 1;
        break;
      }
    }

    //  参照するID順にソートする
    std::sort(StartDrawItr, EndDrawItr,
      [&](const GraphicsCommandPair& _Command0, const GraphicsCommandPair& _Command1)
    {
      return DisplayObjectIDs_[_Command0.second] < DisplayObjectIDs_[_Command1.second];
    });
  }

  Iterator NextItr;
  //  GraphicsCommandFlag::eDraw
  if (!DrawDatas_.empty())
  {
    for (Iterator Itr = StartDrawItr; Itr < GraphicsCommands_.end() - 1;)
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

        Itr = GraphicsCommands_.erase(NextItr);
      }
    }
  }
  //  GraphicsCommandFlag::eDrawIndexed
  else
  {
    for (Iterator Itr = StartDrawItr; Itr < GraphicsCommands_.end() - 1;)
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

        Itr = GraphicsCommands_.erase(NextItr);
      }
    }
  }
}

//--------------------------------------------------

template<class DisplayObject, class InstanceData>
inline void GraphicsCommandList<DisplayObject, InstanceData>::Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState,
  vdl::RasterizerState&& _RasterizerState, vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, InstanceBuffer&& _InstanceBuffer)
{
  pModelManager_ = Engine::Get<IModelManager>();

  BaseGraphicsCommandList::Initialize(_InputLayout, _Topology, std::move(_BlendState), std::move(_DepthStencilState), std::move(_RasterizerState), std::move(_Sampler), std::move(_VertexShader), std::move(_PixelShader), std::move(_InstanceBuffer));
}

template<class DisplayObject, class InstanceData>
inline void GraphicsCommandList<DisplayObject, InstanceData>::SetDrawData(const DisplayObject& _DisplayObject, InstanceData&& _InstanceData)
{
  const vdl::ID DiplayObjectID = _DisplayObject.GetID();
  const Mesh* pMesh = pModelManager_->GetMesh(DiplayObjectID);

  //  必要ステートの設定
  {
    BaseGraphicsCommandList::SetVertexBuffer(pMesh->VertexBuffer);
    BaseGraphicsCommandList::SetIndexBuffer(pMesh->IndexBuffer);

    vdl::ShaderResource ShaderResources[2] = { pMesh->Material.Diffuse, pMesh->Material.NormalMap };
    BaseGraphicsCommandList::SetShaderResources<ShaderType::ePixelShader>(0, 2, ShaderResources);
  }

  std::unique_lock ResetLock(ResetMutex_);
  ConditionVariable_.wait(ResetLock, [&] { return isReady_; });

  std::lock_guard Lock(Mutex_);

  //  現在のステートのコマンドを発行
  BaseGraphicsCommandList::PushGraphicsCommand();

  //  Draw
  {
    if (ReservedDisplayObjects_.find(DiplayObjectID) == ReservedDisplayObjects_.end())
    {
      ReservedDisplayObjects_.insert(std::make_pair(DiplayObjectID, _DisplayObject));
    }

    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kDrawIndexedFlag)), static_cast<vdl::uint>(DrawIndexedDatas_.size()));
    DrawIndexedDatas_.emplace_back(pMesh->IndexCount, 1, pMesh->IndexStart, 0, 0);
    DisplayObjectIDs_.push_back(DiplayObjectID);

    auto& InstanceDatas = Instances_.emplace_back(kInstanceSize);
    ::memmove(InstanceDatas.data(), &_InstanceData, kInstanceSize);

    Sort();
  }
}

//--------------------------------------------------

template<class InstanceData>
inline void GraphicsCommandList<vdl::Texture, InstanceData>::Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState, vdl::RasterizerState&& _RasterizerState,
  vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader, VertexBuffer&& _VertexBuffer, InstanceBuffer&& _InstanceBuffer)
{
  VertexBuffers_.push_back(CurrentVertexBuffer_ = std::move(_VertexBuffer));
  BaseGraphicsCommandList::Initialize(_InputLayout, _Topology, std::move(_BlendState), std::move(_DepthStencilState), std::move(_RasterizerState), std::move(_Sampler), std::move(_VertexShader), std::move(_PixelShader), std::move(_InstanceBuffer));
}

template<class InstanceData>
inline void GraphicsCommandList<vdl::Texture, InstanceData>::SetDrawData(const vdl::Texture& _Texture, InstanceData&& _InstanceData)
{
  //  必要ステートの設定
  BaseGraphicsCommandList::SetShaderResources<ShaderType::ePixelShader>(0, 1, &vdl::ShaderResource(_Texture));

  std::unique_lock ResetLock(ResetMutex_);
  ConditionVariable_.wait(ResetLock, [&] { return isReady_; });

  std::lock_guard Lock(Mutex_);

  //  現在のステートのコマンドを発行
  BaseGraphicsCommandList::PushGraphicsCommand();

  //  Draw
  {
    const vdl::ID DiplayObjectID = _Texture.GetID();

    if (ReservedDisplayObjects_.find(DiplayObjectID) == ReservedDisplayObjects_.end())
    {
      ReservedDisplayObjects_.insert(std::make_pair(DiplayObjectID, _Texture));
    }

    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kDrawFlag)), static_cast<vdl::uint>(DrawDatas_.size()));
    DrawDatas_.emplace_back(4, 1, 0, 0);
    DisplayObjectIDs_.push_back(DiplayObjectID);

    auto& InstanceDatas = Instances_.emplace_back(kInstanceSize);
    ::memmove(InstanceDatas.data(), &_InstanceData, kInstanceSize);

    Sort();
  }
}

//--------------------------------------------------

inline void GraphicsCommandList<vdl::Texture, std::nullptr_t>::Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState,
  vdl::RasterizerState&& _RasterizerState, vdl::Sampler&& _Sampler, vdl::VertexShader&& _VertexShader, vdl::PixelShader&& _PixelShader)
{
  BaseGraphicsCommandList::Initialize(_InputLayout, _Topology, std::move(_BlendState), std::move(_DepthStencilState), std::move(_RasterizerState), std::move(_Sampler), std::move(_VertexShader), std::move(_PixelShader), InstanceBuffer());
}

inline void GraphicsCommandList<vdl::Texture, std::nullptr_t>::SetDrawData(const vdl::Texture& _Texture, DrawIndexedData&& _DrawIndexedData)
{
  //  必要ステートの設定
  BaseGraphicsCommandList::SetShaderResources<ShaderType::ePixelShader>(0, 1, &vdl::ShaderResource(_Texture));

  std::unique_lock ResetLock(ResetMutex_);
  ConditionVariable_.wait(ResetLock, [&] { return isReady_; });

  std::lock_guard Lock(Mutex_);

  //  現在のステートのコマンドを発行
  BaseGraphicsCommandList::PushGraphicsCommand();

  //  Draw
  {
    const vdl::ID DiplayObjectID = _Texture.GetID();

    if (ReservedDisplayObjects_.find(DiplayObjectID) == ReservedDisplayObjects_.end())
    {
      ReservedDisplayObjects_.insert(std::make_pair(DiplayObjectID, _Texture));
    }

    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kDrawIndexedFlag)), static_cast<vdl::uint>(DrawIndexedDatas_.size()));
    DrawIndexedDatas_.emplace_back(std::move(_DrawIndexedData));
    DisplayObjectIDs_.push_back(DiplayObjectID);
  }
}

//--------------------------------------------------

inline void GraphicsCommandList<std::nullptr_t, std::nullptr_t>::Initialize(vdl::InputLayoutType _InputLayout, vdl::TopologyType _Topology, vdl::BlendState&& _BlendState, vdl::DepthStencilState&& _DepthStencilState, vdl::RasterizerState&& _RasterizerState,
  vdl::Sampler&& _Sampler)
{
  BaseGraphicsCommandList::Initialize(_InputLayout, _Topology, std::move(_BlendState), std::move(_DepthStencilState), std::move(_RasterizerState), std::move(_Sampler), vdl::VertexShader(), vdl::PixelShader(), InstanceBuffer());
}

inline void GraphicsCommandList<std::nullptr_t, std::nullptr_t>::SetDrawData(DrawData&& _DrawData)
{
  std::unique_lock ResetLock(ResetMutex_);
  ConditionVariable_.wait(ResetLock, [&] { return isReady_; });

  std::lock_guard Lock(Mutex_);

  //  現在のステートのコマンドを発行
  BaseGraphicsCommandList::PushGraphicsCommand();

  //  Draw
  {
    GraphicsCommands_.emplace_back(static_cast<GraphicsCommandFlag>(static_cast<vdl::uint>(kDrawFlag)), static_cast<vdl::uint>(DrawDatas_.size()));
    DrawDatas_.emplace_back(std::move(_DrawData));
  }
}
