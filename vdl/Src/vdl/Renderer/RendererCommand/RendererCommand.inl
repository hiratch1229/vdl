#include <vdl/Engine.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>

#include <vdl/Math.hpp>

#include <algorithm>

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::Initialize(const vdl::uint2& _WindowSize, const vdl::BlendState& _BlendState, const vdl::DepthStencilState& _DepthStencilState, const vdl::RasterizerState& _RasterizerState,
  const vdl::VertexShader& _VertexShader, const vdl::HullShader& _HullShader, const vdl::DomainShader& _DomainShader, const vdl::GeometryShader& _GeometryShader, const vdl::PixelShader& _PixelShader)
{
  pBufferManager_ = Engine::Get<IBufferManager>();

  Scissors_.push_back(vdl::Scissor(0, _WindowSize));
  Viewports_.push_back(vdl::Viewport(0, _WindowSize));
  BlendStates_.push_back(_BlendState);
  DepthStencilStates_.push_back(_DepthStencilState);
  RasterizerStates_.push_back(_RasterizerState);
  VertexShaders_.push_back(_VertexShader);
  HullShaders_.push_back(_HullShader);
  DomainShaders_.push_back(_DomainShader);
  GeometryShaders_.push_back(_GeometryShader);
  PixelShaders_.push_back(_PixelShader);

  for (auto& Samplers : Samplers_) { Samplers.resize(1); }
  for (auto& TextureIDs : TextureIDs_) { TextureIDs.resize(1); }
  for (auto& ConstantBufferIDs : ConstantBufferIDs_) { ConstantBufferIDs.resize(1); }

  Reset();
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::Reset()
{
  RendererCommands_.clear();
  StateChangeFlags_.Clear();
  Instances_.clear();

  Scissors_ = { Scissors_.back() };
  RendererCommands_.emplace_back(RendererCommnadType::eSetScissor, 0);

  Viewports_ = { Viewports_.back() };
  RendererCommands_.emplace_back(RendererCommnadType::eSetViewport, 0);

  BlendStates_ = { BlendStates_.back() };
  RendererCommands_.emplace_back(RendererCommnadType::eSetBlendState, 0);

  DepthStencilStates_ = { DepthStencilStates_.back() };
  RendererCommands_.emplace_back(RendererCommnadType::eSetDepthStencilState, 0);

  RasterizerStates_ = { RasterizerStates_.back() };
  RendererCommands_.emplace_back(RendererCommnadType::eSetRasterizerState, 0);

  VertexShaders_ = { VertexShaders_.back() };
  RendererCommands_.emplace_back(RendererCommnadType::eSetVertexShader, 0);

  HullShaders_ = { HullShaders_.back() };
  RendererCommands_.emplace_back(RendererCommnadType::eSetHullShader, 0);

  DomainShaders_ = { DomainShaders_.back() };
  RendererCommands_.emplace_back(RendererCommnadType::eSetDomainShader, 0);

  GeometryShaders_ = { GeometryShaders_.back() };
  RendererCommands_.emplace_back(RendererCommnadType::eSetGeometryShader, 0);

  PixelShaders_ = { PixelShaders_.back() };
  RendererCommands_.emplace_back(RendererCommnadType::eSetPixelShader, 0);

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kShaderTypes; ++ShaderTypeCount)
  {
    auto& Samplers = Samplers_[ShaderTypeCount];

    Samplers = { Samplers.back() };
    RendererCommands_.emplace_back(static_cast<RendererCommnadType>(static_cast<vdl::uint>(RendererCommnadType::eSetVertexStageSampler) + ShaderTypeCount), 0);
  }

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kShaderTypes; ++ShaderTypeCount)
  {
    auto& TextureIDs = TextureIDs_[ShaderTypeCount];

    TextureIDs = { TextureIDs.back() };
    RendererCommands_.emplace_back(static_cast<RendererCommnadType>(static_cast<vdl::uint>(RendererCommnadType::eSetVertexStageTexture) + ShaderTypeCount), 0);
  }

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kShaderTypes; ++ShaderTypeCount)
  {
    auto& ConstantBufferIDs = ConstantBufferIDs_[ShaderTypeCount];

    ConstantBufferIDs = { ConstantBufferIDs.back() };
    RendererCommands_.emplace_back(static_cast<RendererCommnadType>(static_cast<vdl::uint>(RendererCommnadType::eSetVertexStageConstantBuffer) + ShaderTypeCount), 0);
  }

  ReservedDisplayObjects_.clear();

  //  ReservedTexturesの不要な要素の削除
  {
    std::vector<vdl::ID> EraseTextureIDs;
    for (auto& ReservedTextures : ReservedTextures_)
    {
      EraseTextureIDs.push_back(ReservedTextures.first);
    }

    EraseTextureIDs.erase(std::remove_if(EraseTextureIDs.begin(), EraseTextureIDs.end(),
      [&](const vdl::ID& _ID)->bool
    {
      for (auto& TextureIDs : TextureIDs_)
      {
        for (auto& TextureID : TextureIDs[0])
        {
          if (_ID == TextureID)
          {
            return false;
          }
        }
      }

      return true;
    }), EraseTextureIDs.end());

    for (auto& EraseTextureID : EraseTextureIDs)
    {
      ReservedTextures_.erase(EraseTextureID);
    }
  }

  //  ReservedConstantBuffersの不要な要素の削除
  {
    std::vector<vdl::ID> EraseConstantBufferIDs;
    for (auto& ReservedConstantBuffers : ReservedConstantBuffers_)
    {
      EraseConstantBufferIDs.push_back(ReservedConstantBuffers.first);
    }

    EraseConstantBufferIDs.erase(std::remove_if(EraseConstantBufferIDs.begin(), EraseConstantBufferIDs.end(),
      [&](const vdl::ID& _ID)->bool
    {
      for (auto& ConstantBufferIDs : ConstantBufferIDs_)
      {
        for (auto& ConstantBufferID : ConstantBufferIDs[0])
        {
          if (_ID == ConstantBufferID)
          {
            return false;
          }
        }
      }

      for (auto& LastConstantBufferIDs : LastConstantBufferIDs_)
      {
        for (auto& LastConstantBufferID : LastConstantBufferIDs)
        {
          if (_ID == LastConstantBufferID)
          {
            return false;
          }
        }
      }

      for (auto& CurrentConstantBufferIDs : CurrentConstantBufferIDs_)
      {
        for (auto& CurrentConstantBufferID : CurrentConstantBufferIDs)
        {
          if (_ID == CurrentConstantBufferID)
          {
            return false;
          }
        }
      }

      return true;
    }), EraseConstantBufferIDs.end());

    for (auto& EraseConstantBufferID : EraseConstantBufferIDs)
    {
      ReservedConstantBuffers_.erase(EraseConstantBufferID);
    }
  }
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushDrawData(const DisplayObject& _DisplayObject, InstanceData&& _InstanceData)
{
  if (StateChangeFlags_.Has(RendererCommnadType::eSetScissor))
  {
    RendererCommands_.emplace_back(RendererCommnadType::eSetScissor, static_cast<vdl::uint>(Scissors_.size()));
    Scissors_.push_back(CurrentScissor_);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetViewport))
  {
    RendererCommands_.emplace_back(RendererCommnadType::eSetViewport, static_cast<vdl::uint>(Viewports_.size()));
    Viewports_.push_back(CurrentViewport_);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetBlendState))
  {
    RendererCommands_.emplace_back(RendererCommnadType::eSetBlendState, static_cast<vdl::uint>(BlendStates_.size()));
    BlendStates_.push_back(CurrentBlendState_);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetDepthStencilState))
  {
    RendererCommands_.emplace_back(RendererCommnadType::eSetDepthStencilState, static_cast<vdl::uint>(DepthStencilStates_.size()));
    DepthStencilStates_.push_back(CurrentDepthStencilState_);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetRasterizerState))
  {
    RendererCommands_.emplace_back(RendererCommnadType::eSetRasterizerState, static_cast<vdl::uint>(RasterizerStates_.size()));
    RasterizerStates_.push_back(CurrentRasterizerState_);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetVertexShader))
  {
    RendererCommands_.emplace_back(RendererCommnadType::eSetVertexShader, static_cast<vdl::uint>(VertexShaders_.size()));
    VertexShaders_.push_back(CurrentVertexShader_);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetHullShader))
  {
    RendererCommands_.emplace_back(RendererCommnadType::eSetHullShader, static_cast<vdl::uint>(HullShaders_.size()));
    HullShaders_.push_back(CurrentHullShader_);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetDomainShader))
  {
    RendererCommands_.emplace_back(RendererCommnadType::eSetDomainShader, static_cast<vdl::uint>(DomainShaders_.size()));
    DomainShaders_.push_back(CurrentDomainShader_);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetGeometryShader))
  {
    RendererCommands_.emplace_back(RendererCommnadType::eSetGeometryShader, static_cast<vdl::uint>(GeometryShaders_.size()));
    GeometryShaders_.push_back(CurrentGeometryShader_);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetPixelShader))
  {
    RendererCommands_.emplace_back(RendererCommnadType::eSetPixelShader, static_cast<vdl::uint>(PixelShaders_.size()));
    PixelShaders_.push_back(CurrentPixelShader_);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetVertexStageSampler))
  {
    auto& Sampler = Samplers_[static_cast<vdl::uint>(ShaderType::eVertexShader)];

    RendererCommands_.emplace_back(RendererCommnadType::eSetVertexStageSampler, static_cast<vdl::uint>(Sampler.size()));
    Sampler.push_back(CurrentSamplers_[static_cast<vdl::uint>(ShaderType::eVertexShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetHullStageSampler))
  {
    auto& Sampler = Samplers_[static_cast<vdl::uint>(ShaderType::eHullShader)];

    RendererCommands_.emplace_back(RendererCommnadType::eSetHullStageSampler, static_cast<vdl::uint>(Sampler.size()));
    Sampler.push_back(CurrentSamplers_[static_cast<vdl::uint>(ShaderType::eHullShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetDomainStageSampler))
  {
    auto& Sampler = Samplers_[static_cast<vdl::uint>(ShaderType::eDomainShader)];

    RendererCommands_.emplace_back(RendererCommnadType::eSetDomainStageSampler, static_cast<vdl::uint>(Sampler.size()));
    Sampler.push_back(CurrentSamplers_[static_cast<vdl::uint>(ShaderType::eDomainShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetGeometryStageSampler))
  {
    auto& Sampler = Samplers_[static_cast<vdl::uint>(ShaderType::eGeometryShader)];

    RendererCommands_.emplace_back(RendererCommnadType::eSetGeometryStageSampler, static_cast<vdl::uint>(Sampler.size()));
    Sampler.push_back(CurrentSamplers_[static_cast<vdl::uint>(ShaderType::eGeometryShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetPixelStageSampler))
  {
    auto& Sampler = Samplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)];

    RendererCommands_.emplace_back(RendererCommnadType::eSetPixelStageSampler, static_cast<vdl::uint>(Sampler.size()));
    Sampler.push_back(CurrentSamplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetVertexStageTexture))
  {
    auto& TextureIDs = TextureIDs_[static_cast<vdl::uint>(ShaderType::eVertexShader)];

    RendererCommands_.emplace_back(RendererCommnadType::eSetVertexStageTexture, static_cast<vdl::uint>(TextureIDs.size()));
    TextureIDs.push_back(CurrentTextureIDs_[static_cast<vdl::uint>(ShaderType::eVertexShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetHullStageTexture))
  {
    auto& TextureIDs = TextureIDs_[static_cast<vdl::uint>(ShaderType::eHullShader)];

    RendererCommands_.emplace_back(RendererCommnadType::eSetHullStageTexture, static_cast<vdl::uint>(TextureIDs.size()));
    TextureIDs.push_back(CurrentTextureIDs_[static_cast<vdl::uint>(ShaderType::eHullShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetDomainStageTexture))
  {
    auto& TextureIDs = TextureIDs_[static_cast<vdl::uint>(ShaderType::eDomainShader)];

    RendererCommands_.emplace_back(RendererCommnadType::eSetDomainStageTexture, static_cast<vdl::uint>(TextureIDs.size()));
    TextureIDs.push_back(CurrentTextureIDs_[static_cast<vdl::uint>(ShaderType::eDomainShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetGeometryStageTexture))
  {
    auto& TextureIDs = TextureIDs_[static_cast<vdl::uint>(ShaderType::eGeometryShader)];

    RendererCommands_.emplace_back(RendererCommnadType::eSetGeometryStageTexture, static_cast<vdl::uint>(TextureIDs.size()));
    TextureIDs.push_back(CurrentTextureIDs_[static_cast<vdl::uint>(ShaderType::eGeometryShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommnadType::eSetPixelStageTexture))
  {
    auto& TextureIDs = TextureIDs_[static_cast<vdl::uint>(ShaderType::ePixelShader)];

    RendererCommands_.emplace_back(RendererCommnadType::eSetPixelStageTexture, static_cast<vdl::uint>(TextureIDs.size()));
    TextureIDs.push_back(CurrentTextureIDs_[static_cast<vdl::uint>(ShaderType::ePixelShader)]);
  }

  //  SetConstantBuffer
  {
    auto IssueRendererCommandSetConstantBuffer = [this](RendererCommnadType _RendererCommnadType, ShaderType _ShaderType)->void
    {
      auto& CurrentConstantBufferIDs = CurrentConstantBufferIDs_[static_cast<vdl::uint>(_ShaderType)];
      auto& ConstantBufferIDs = ConstantBufferIDs_[static_cast<vdl::uint>(_ShaderType)];
      auto& LastConstantBufferIDs = LastConstantBufferIDs_[static_cast<vdl::uint>(_ShaderType)];

      std::remove_reference<decltype(CurrentConstantBufferIDs)>::type TempConstantBufferIDs = ConstantBufferIDs.back();
      {
        const size_t CurrentConstantBufferNum = CurrentConstantBufferIDs.size();
        const size_t LastConstantBufferNum = LastConstantBufferIDs.size();

        if (TempConstantBufferIDs.size() < CurrentConstantBufferNum)
        {
          TempConstantBufferIDs.resize(CurrentConstantBufferNum);
        }

        size_t ConstantBufferCount = 0;
        while (ConstantBufferCount < LastConstantBufferNum)
        {
          const vdl::ID CurrentConstantBufferID = CurrentConstantBufferIDs[ConstantBufferCount];
          const vdl::ID LastConstantBufferID = LastConstantBufferIDs[ConstantBufferCount];

          const vdl::Detail::ConstantBufferData& CurrentConstantBuffer = ReservedConstantBuffers_.at(CurrentConstantBufferID);
          const vdl::Detail::ConstantBufferData& BeforeConstantBuffer = ReservedConstantBuffers_.at(TempConstantBufferIDs[ConstantBufferCount]);

          if (CurrentConstantBufferID != LastConstantBufferID
            || ::memcmp(CurrentConstantBuffer.GetData(), BeforeConstantBuffer.GetData(), CurrentConstantBuffer.GetSize()) != 0)
          {
            vdl::Detail::ConstantBufferData ConstantBuffer = std::move(pBufferManager_->CloneConstantBuffer(CurrentConstantBuffer));
            const vdl::ID ConstantBufferID = ConstantBuffer.GetID();

            ReservedConstantBuffers_.insert(std::make_pair(ConstantBufferID, std::move(ConstantBuffer)));
            TempConstantBufferIDs[ConstantBufferCount] = ConstantBufferID;

            StateChangeFlags_.Set(_RendererCommnadType);
          }

          ++ConstantBufferCount;
        }
        while (ConstantBufferCount < CurrentConstantBufferNum)
        {
          vdl::Detail::ConstantBufferData ConstantBuffer = std::move(pBufferManager_->CloneConstantBuffer(ReservedConstantBuffers_.at(CurrentConstantBufferIDs[ConstantBufferCount])));
          const vdl::ID ConstantBufferID = ConstantBuffer.GetID();

          ReservedConstantBuffers_.insert(std::make_pair(ConstantBufferID, std::move(ConstantBuffer)));
          TempConstantBufferIDs[ConstantBufferCount] = ConstantBufferID;

          StateChangeFlags_.Set(_RendererCommnadType);

          ++ConstantBufferCount;
        }
      }

      if (StateChangeFlags_.Has(_RendererCommnadType))
      {
        RendererCommands_.emplace_back(_RendererCommnadType, static_cast<vdl::uint>(ConstantBufferIDs.size()));
        LastConstantBufferIDs = CurrentConstantBufferIDs;
        ConstantBufferIDs.push_back(std::move(TempConstantBufferIDs));
      }
    };

    IssueRendererCommandSetConstantBuffer(RendererCommnadType::eSetVertexStageConstantBuffer, ShaderType::eVertexShader);

    IssueRendererCommandSetConstantBuffer(RendererCommnadType::eSetHullStageConstantBuffer, ShaderType::eHullShader);

    IssueRendererCommandSetConstantBuffer(RendererCommnadType::eSetDomainStageConstantBuffer, ShaderType::eDomainShader);

    IssueRendererCommandSetConstantBuffer(RendererCommnadType::eSetGeometryStageConstantBuffer, ShaderType::eGeometryShader);

    IssueRendererCommandSetConstantBuffer(RendererCommnadType::eSetPixelStageConstantBuffer, ShaderType::ePixelShader);
  }

  //  Draw
  {
    const vdl::ID DiplayObjectID = _DisplayObject.GetID();

    if (ReservedDisplayObjects_.find(DiplayObjectID) == ReservedDisplayObjects_.end())
    {
      ReservedDisplayObjects_.insert(std::make_pair(DiplayObjectID, _DisplayObject));
    }

    RendererCommands_.emplace_back(RendererCommnadType::eDraw, static_cast<vdl::uint>(Instances_.size()));
    Instances_.emplace_back(std::move(_InstanceData));
  }

  StateChangeFlags_.Clear();
}

#define SetState(RendererCommnadType, StateName)\
if (StateChangeFlags_.Has(RendererCommnadType))\
{\
  Current##StateName##_ = _##StateName;\
  \
  if (Current##StateName##_ == StateName##s_.back())\
  {\
    StateChangeFlags_.Cancel(RendererCommnadType);\
  }\
}\
else\
{\
  if (Current##StateName##_ != _##StateName)\
  {\
    Current##StateName##_ = _##StateName;\
    StateChangeFlags_.Set(RendererCommnadType);\
  }\
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushScissor(const vdl::Scissor& _Scissor)
{
  SetState(RendererCommnadType::eSetScissor, Scissor)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushViewport(const vdl::Viewport& _Viewport)
{
  SetState(RendererCommnadType::eSetViewport, Viewport)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushBlendState(const vdl::BlendState& _BlendState)
{
  SetState(RendererCommnadType::eSetBlendState, BlendState)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushDepthStencilState(const vdl::DepthStencilState& _DepthStencilState)
{
  SetState(RendererCommnadType::eSetDepthStencilState, DepthStencilState)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushRasterizerState(const vdl::RasterizerState& _RasterizerState)
{
  SetState(RendererCommnadType::eSetRasterizerState, RasterizerState)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushVertexShader(const vdl::VertexShader& _VertexShader)
{
  SetState(RendererCommnadType::eSetVertexShader, VertexShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushHullShader(const vdl::HullShader& _HullShader)
{
  SetState(RendererCommnadType::eSetHullShader, HullShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushDomainShader(const vdl::DomainShader& _DomainShader)
{
  SetState(RendererCommnadType::eSetDomainShader, DomainShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushGeometryShader(const vdl::GeometryShader& _GeometryShader)
{
  SetState(RendererCommnadType::eSetGeometryShader, GeometryShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushPixelShader(const vdl::PixelShader& _PixelShader)
{
  SetState(RendererCommnadType::eSetPixelShader, PixelShader)
}

#undef SetState

#define SetSampler(RendererCommnadType, ShaderType)\
Samplers& CurrentSamplers = CurrentSamplers_[static_cast<vdl::uint>(ShaderType)];\
\
if (CurrentSamplers.size() <= _Slot)\
{\
  CurrentSamplers.resize(_Slot + 1);\
  StateChangeFlags_.Set(RendererCommnadType);\
}\
\
if (StateChangeFlags_.Has(RendererCommnadType))\
{\
  CurrentSamplers[_Slot] = _Sampler;\
  \
  Samplers& LastSamplers = Samplers_[static_cast<vdl::uint>(ShaderType)].back();\
  \
  const size_t CurrentSamplerNum = CurrentSamplers_.size();\
  if (CurrentSamplerNum != LastSamplers.size())\
  {\
    return;\
  }\
  \
  for (size_t SamplerCount = 0; SamplerCount < CurrentSamplerNum; ++SamplerCount)\
  {\
    if (CurrentSamplers[SamplerCount] != LastSamplers[SamplerCount])\
    {\
      return;\
    }\
  }\
  \
  StateChangeFlags_.Cancel(RendererCommnadType);\
}\
else\
{\
  if (CurrentSamplers[_Slot] != _Sampler)\
  {\
    CurrentSamplers[_Slot] = _Sampler;\
    StateChangeFlags_.Set(RendererCommnadType);\
  }\
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushVertexStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot)
{
  SetSampler(RendererCommnadType::eSetVertexStageSampler, ShaderType::eVertexShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushHullStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot)
{
  SetSampler(RendererCommnadType::eSetHullStageSampler, ShaderType::eHullShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushDomainStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot)
{
  SetSampler(RendererCommnadType::eSetDomainStageSampler, ShaderType::eDomainShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushGeometryStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot)
{
  SetSampler(RendererCommnadType::eSetGeometryStageSampler, ShaderType::eGeometryShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushPixelStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot)
{
  SetSampler(RendererCommnadType::eSetPixelStageSampler, ShaderType::ePixelShader)
}

#undef SetSampler

#define SetTexture(RendererCommnadType, ShaderType)\
const vdl::ID TextureID = _Texture.GetID();\
\
if (ReservedTextures_.find(TextureID) == ReservedTextures_.end())\
{\
  ReservedTextures_.insert(std::make_pair(TextureID, _Texture));\
}\
\
TextureIDs& CurrentTextureIDs = CurrentTextureIDs_[static_cast<vdl::uint>(ShaderType)];\
if (CurrentTextureIDs.size() <= _Slot)\
{\
  CurrentTextureIDs.resize(_Slot + 1);\
  StateChangeFlags_.Set(RendererCommnadType);\
}\
\
if (StateChangeFlags_.Has(RendererCommnadType))\
{\
  CurrentTextureIDs[_Slot] = TextureID;\
  \
  TextureIDs& LastTextureIDs = TextureIDs_[static_cast<vdl::uint>(ShaderType)].back();\
  \
  const size_t CurrentTextureIDNum = CurrentTextureIDs.size();\
  if (CurrentTextureIDNum != LastTextureIDs.size())\
  {\
    return;\
  }\
  \
  for (size_t TextureIDCount = 0; TextureIDCount < CurrentTextureIDNum; ++TextureIDCount)\
  {\
    if (CurrentTextureIDs[TextureIDCount] != LastTextureIDs[TextureIDCount])\
    {\
      return;\
    }\
  }\
  \
  StateChangeFlags_.Cancel(RendererCommnadType);\
}\
else\
{\
  if (CurrentTextureIDs[_Slot] != TextureID)\
  {\
    CurrentTextureIDs[_Slot] = TextureID;\
    StateChangeFlags_.Set(RendererCommnadType);\
  }\
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushVertexStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot)
{
  SetTexture(RendererCommnadType::eSetVertexStageTexture, ShaderType::eVertexShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushHullStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot)
{
  SetTexture(RendererCommnadType::eSetHullStageTexture, ShaderType::eHullShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushDomainStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot)
{
  SetTexture(RendererCommnadType::eSetDomainStageTexture, ShaderType::eDomainShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushGeometryStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot)
{
  SetTexture(RendererCommnadType::eSetGeometryStageTexture, ShaderType::eGeometryShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushPixelStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot)
{
  SetTexture(RendererCommnadType::eSetPixelStageTexture, ShaderType::ePixelShader)
}

#undef SetTexture

#define SetConstantBuffer(RendererCommnadType, ShaderType)\
const vdl::ID ConstantBufferID = _ConstantBuffer.GetID();\
\
if (ReservedConstantBuffers_.find(ConstantBufferID) == ReservedConstantBuffers_.end())\
{\
  ReservedConstantBuffers_.insert(std::make_pair(ConstantBufferID, _ConstantBuffer));\
}\
\
ConstantBufferIDs& CurrentConstantBufferIDs = CurrentConstantBufferIDs_[static_cast<vdl::uint>(ShaderType)];\
if (CurrentConstantBufferIDs.size() <= _Slot)\
{\
  CurrentConstantBufferIDs.resize(_Slot + 1);\
  StateChangeFlags_.Set(RendererCommnadType);\
}\
\
if (StateChangeFlags_.Has(RendererCommnadType))\
{\
  CurrentConstantBufferIDs[_Slot] = ConstantBufferID;\
  \
  ConstantBufferIDs& LastConstantBufferIDs = LastConstantBufferIDs_[static_cast<vdl::uint>(ShaderType)];\
  \
  const size_t CurrentConstantBufferIDNum = CurrentConstantBufferIDs.size();\
  if (CurrentConstantBufferIDNum != LastConstantBufferIDs.size())\
  {\
    return;\
  }\
  \
  for (size_t ConstantBufferIDCount = 0; ConstantBufferIDCount < CurrentConstantBufferIDNum; ++ConstantBufferIDCount)\
  {\
    if (CurrentConstantBufferIDs[ConstantBufferIDCount] != LastConstantBufferIDs[ConstantBufferIDCount])\
    {\
      return;\
    }\
  }\
  \
  StateChangeFlags_.Cancel(RendererCommnadType);\
}\
else\
{\
  if (CurrentConstantBufferIDs[_Slot] != ConstantBufferID)\
  {\
    CurrentConstantBufferIDs[_Slot] = ConstantBufferID;\
    StateChangeFlags_.Set(RendererCommnadType);\
  }\
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushVertexStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot)
{
  SetConstantBuffer(RendererCommnadType::eSetVertexStageConstantBuffer, ShaderType::eVertexShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushHullStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot)
{
  SetConstantBuffer(RendererCommnadType::eSetHullStageConstantBuffer, ShaderType::eHullShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushDomainStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot)
{
  SetConstantBuffer(RendererCommnadType::eSetDomainStageConstantBuffer, ShaderType::eDomainShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushGeometryStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot)
{
  SetConstantBuffer(RendererCommnadType::eSetGeometryStageConstantBuffer, ShaderType::eGeometryShader)
}

template<class DisplayObject, class InstanceData>
void RendererCommandList<DisplayObject, InstanceData>::PushPixelStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot)
{
  SetConstantBuffer(RendererCommnadType::eSetPixelStageConstantBuffer, ShaderType::ePixelShader)
}

#undef SetConstantBuffer
