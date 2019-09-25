#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>
#include <vdl/ModelManager/IModelManager.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>

#include <vdl/Model.hpp>

#include <vdl/Math.hpp>

#include <algorithm>

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::Initialize(const vdl::Scissor& _Scissor, const vdl::Viewport& _Viewport, const vdl::BlendState& _BlendState, const vdl::DepthStencilState& _DepthStencilState, const vdl::RasterizerState& _RasterizerState,
  const vdl::Sampler& _Sampler, const vdl::VertexShader& _VertexShader, const vdl::PixelShader& _PixelShader)
{
  pDevice_ = Engine::Get<IDevice>();
  if constexpr (std::is_same<DisplayObject, vdl::StaticMesh>::value || std::is_same<DisplayObject, vdl::SkinnedMesh>::value)
  {
    pModelManager_ = Engine::Get<IModelManager>();
  }
  pBufferManager_ = Engine::Get<IBufferManager>();

  Scissors_.push_back(CurrentScissor_ = _Scissor);
  Viewports_.push_back(CurrentViewport_ = _Viewport);
  BlendStates_.push_back(CurrentBlendState_ = _BlendState);
  DepthStencilStates_.push_back(CurrentDepthStencilState_ = _DepthStencilState);
  RasterizerStates_.push_back(CurrentRasterizerState_ = _RasterizerState);
  VertexShaders_.push_back(CurrentVertexShader_ = _VertexShader);
  PixelShaders_.push_back(CurrentPixelShader_ = _PixelShader);
  HullShaders_.emplace_back(vdl::HullShader());
  DomainShaders_.emplace_back(vdl::DomainShader());
  GeometryShaders_.emplace_back(vdl::GeometryShader());

  for (auto& Samplers : Samplers_) { Samplers.resize(1); }
  for (auto& TextureIDs : TextureIDs_) { TextureIDs.resize(1); }
  for (auto& ConstantBufferIDs : ConstantBufferIDs_) { ConstantBufferIDs.resize(1); }

  Samplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][0].resize(1);
  Samplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][0][0] = _Sampler;

  Reset();
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::Reset()
{
  RendererCommands_.clear();
  StateChangeFlags_.Clear();
  Instances_.clear();
  DisplayObjectIDs_.clear();

  Scissors_ = { Scissors_.back() };
  RendererCommands_.emplace_back(RendererCommandType::eSetScissor, 0);

  Viewports_ = { Viewports_.back() };
  RendererCommands_.emplace_back(RendererCommandType::eSetViewport, 0);

  BlendStates_ = { BlendStates_.back() };
  RendererCommands_.emplace_back(RendererCommandType::eSetBlendState, 0);

  DepthStencilStates_ = { DepthStencilStates_.back() };
  RendererCommands_.emplace_back(RendererCommandType::eSetDepthStencilState, 0);

  RasterizerStates_ = { RasterizerStates_.back() };
  RendererCommands_.emplace_back(RendererCommandType::eSetRasterizerState, 0);

  VertexShaders_ = { VertexShaders_.back() };
  RendererCommands_.emplace_back(RendererCommandType::eSetVertexShader, 0);

  HullShaders_ = { HullShaders_.back() };
  RendererCommands_.emplace_back(RendererCommandType::eSetHullShader, 0);

  DomainShaders_ = { DomainShaders_.back() };
  RendererCommands_.emplace_back(RendererCommandType::eSetDomainShader, 0);

  GeometryShaders_ = { GeometryShaders_.back() };
  RendererCommands_.emplace_back(RendererCommandType::eSetGeometryShader, 0);

  PixelShaders_ = { PixelShaders_.back() };
  RendererCommands_.emplace_back(RendererCommandType::eSetPixelShader, 0);

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kShaderTypes; ++ShaderTypeCount)
  {
    auto& Samplers = Samplers_[ShaderTypeCount];

    Samplers = { Samplers.back() };
    RendererCommands_.emplace_back(static_cast<RendererCommandType>(static_cast<vdl::uint>(RendererCommandType::eSetVertexStageSampler) + ShaderTypeCount), 0);
  }

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kShaderTypes; ++ShaderTypeCount)
  {
    auto& TextureIDs = TextureIDs_[ShaderTypeCount];

    TextureIDs = { TextureIDs.back() };
    RendererCommands_.emplace_back(static_cast<RendererCommandType>(static_cast<vdl::uint>(RendererCommandType::eSetVertexStageTexture) + ShaderTypeCount), 0);
  }

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kShaderTypes; ++ShaderTypeCount)
  {
    auto& ConstantBufferIDs = ConstantBufferIDs_[ShaderTypeCount];

    ConstantBufferIDs = { ConstantBufferIDs.back() };
    RendererCommands_.emplace_back(static_cast<RendererCommandType>(static_cast<vdl::uint>(RendererCommandType::eSetVertexStageConstantBuffer) + ShaderTypeCount), 0);
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
            return true;
          }
        }
      }

      return false;
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
            return true;
          }
        }
      }

      for (auto& LastConstantBufferIDs : LastConstantBufferIDs_)
      {
        for (auto& LastConstantBufferID : LastConstantBufferIDs)
        {
          if (_ID == LastConstantBufferID)
          {
            return true;
          }
        }
      }

      for (auto& CurrentConstantBufferIDs : CurrentConstantBufferIDs_)
      {
        for (auto& CurrentConstantBufferID : CurrentConstantBufferIDs)
        {
          if (_ID == CurrentConstantBufferID)
          {
            return true;
          }
        }
      }

      return false;
    }), EraseConstantBufferIDs.end());

    for (auto& EraseConstantBufferID : EraseConstantBufferIDs)
    {
      ReservedConstantBuffers_.erase(EraseConstantBufferID);
    }
  }
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::Adjust()
{
  Thread_ = std::thread([this]()->void
  {
    bool canSort = false;
    vdl::uint StartDrawCallIndex;
    vdl::uint ContinuousDrawCallNum = 0;

    const vdl::uint CommandNum = static_cast<vdl::uint>(RendererCommands_.size());
    for (vdl::uint CommandCount = 0; CommandCount < CommandNum; ++CommandCount)
    {
      const RendererCommandPair& RendererCommand = RendererCommands_[CommandCount];
      assert(RendererCommand.first != RendererCommandType::eNum);

      switch (RendererCommand.first)
      {
      case RendererCommandType::eDraw:
        //  ソート出来ない時はスルーする
        if (!canSort)
        {
          continue;
        }

        //  ドローコールの回数を記録
        if (ContinuousDrawCallNum++ == 0)
        {
          StartDrawCallIndex = CommandCount;
        }
        break;
      case RendererCommandType::eSetDepthStencilState:
        //  Depthがオンの時にソート可能
        canSort = DepthStencilStates_[RendererCommand.second].DepthEnable;
        //break;
      default:
        //  2つ以上連続でドローコールがある時にソートする
        if (ContinuousDrawCallNum > 1)
        {
          const auto& StartIterator = RendererCommands_.begin() + StartDrawCallIndex;
          std::sort(StartIterator, StartIterator + ContinuousDrawCallNum,
            [this](const RendererCommandPair& _Command0, const RendererCommandPair& _Command1)
          {
            assert(_Command0.first == RendererCommandType::eDraw && _Command1.first == RendererCommandType::eDraw);

            //  参照するID順にソートする
            return DisplayObjectIDs_[_Command0.second] < DisplayObjectIDs_[_Command1.second];
          });
        }
        ContinuousDrawCallNum = 0;
        break;
      }
    }
  });
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::Flush(IDeviceContext* _pDeviceContext, IBuffer* _pInstanceBuffer)
{
  //  ソートが終わるまで待機
  Thread_.join();

  const vdl::uint RendererCommandNum = static_cast<vdl::uint>(RendererCommands_.size());
  for (vdl::uint RendererCommandCount = 0; RendererCommandCount < RendererCommandNum; ++RendererCommandCount)
  {
    const RendererCommandPair& RendererCommand = RendererCommands_[RendererCommandCount];

    switch (RendererCommand.first)
    {
    case RendererCommandType::eDraw:
    {
      enum class DrawStateType
      {
        eContinue,
        eFlush,
        eEnd
      };

      std::vector<InstanceData> Instances;
      vdl::uint StartDrawCallIndex = RendererCommandCount;
      vdl::uint ContinuousDrawCallNum = 0;
      DrawStateType DrawState;

      for (; RendererCommandCount < RendererCommandNum; ++RendererCommandCount)
      {
        DrawState = DrawStateType::eContinue;
        ++ContinuousDrawCallNum;

        const vdl::ID CurrentDisplayObjectID = DisplayObjectIDs_[RendererCommands_[RendererCommandCount].second];
        const vdl::uint NextRendererCommandCount = RendererCommandCount + 1;

        if (NextRendererCommandCount == RendererCommandNum)
        {
          DrawState = DrawStateType::eEnd;
        }
        else
        {
          const RendererCommandPair& NextRendererCommand = RendererCommands_[NextRendererCommandCount];

          if (NextRendererCommand.first != RendererCommandType::eDraw)
          {
            DrawState = DrawStateType::eEnd;
          }
          else if (CurrentDisplayObjectID != DisplayObjectIDs_[NextRendererCommand.second])
          {
            DrawState = DrawStateType::eFlush;
          }
        }

        if (DrawState != DrawStateType::eContinue)
        {
          Instances.resize(ContinuousDrawCallNum);
          for (vdl::uint InstanceCount = 0; InstanceCount < ContinuousDrawCallNum; ++InstanceCount)
          {
            assert(RendererCommands_[StartDrawCallIndex + InstanceCount].first == RendererCommandType::eDraw);
            Instances[InstanceCount] = std::move(Instances_[RendererCommands_[StartDrawCallIndex + InstanceCount].second]);
          }

          pDevice_->WriteMemory(_pInstanceBuffer, Instances.data(), sizeof(InstanceData) * ContinuousDrawCallNum);
          _pDeviceContext->SetInstanceBuffer(_pInstanceBuffer);

          if constexpr (std::is_same<DisplayObject, vdl::Texture>::value)
          {
            _pDeviceContext->PSSetTextures(0, 1, &ReservedDisplayObjects_[CurrentDisplayObjectID]);

            _pDeviceContext->Draw(4, ContinuousDrawCallNum, 0, 0);
          }
          else if constexpr (std::is_same<DisplayObject, vdl::StaticMesh>::value)
          {
            const Mesh* pMesh = pModelManager_->GetMesh(CurrentDisplayObjectID);

            _pDeviceContext->SetVertexBuffer(pMesh->pVertexBuffer.get());
            _pDeviceContext->SetIndexBuffer(pMesh->pIndexBuffer.get());

            for (auto& Material : pMesh->Materials)
            {
              _pDeviceContext->PSSetTextures(0, 1, &Material.Diffuse);

              _pDeviceContext->DrawIndexed(Material.IndexCount, ContinuousDrawCallNum, Material.IndexStart, 0, 0);
            }
          }
          else if constexpr (std::is_same<DisplayObject, vdl::SkinnedMesh>::value)
          {
            //  TODO
          }

          if (DrawState == DrawStateType::eEnd)
          {
            break;
          }

          ContinuousDrawCallNum = 0;
          StartDrawCallIndex = NextRendererCommandCount;
          //Instances.clear();
        }
      }
    }
    break;
    case RendererCommandType::eSetScissor:
    {
      const vdl::Scissor& Scissor = Scissors_[RendererCommand.second];

      _pDeviceContext->SetScissor(Scissor);
    }
    break;
    case RendererCommandType::eSetViewport:
    {
      const vdl::Viewport& Viewport = Viewports_[RendererCommand.second];

      _pDeviceContext->SetViewport(Viewport);
    }
    break;
    case RendererCommandType::eSetBlendState:
    {
      const vdl::BlendState& BlendState = BlendStates_[RendererCommand.second];

      _pDeviceContext->SetBlendState(BlendState);
    }
    break;
    case RendererCommandType::eSetDepthStencilState:
    {
      const vdl::DepthStencilState& DepthStencilState = DepthStencilStates_[RendererCommand.second];

      _pDeviceContext->SetDepthStencilState(DepthStencilState);
    }
    break;
    case RendererCommandType::eSetRasterizerState:
    {
      const vdl::RasterizerState& RasterizerState = RasterizerStates_[RendererCommand.second];

      _pDeviceContext->SetRasterizerState(RasterizerState);
    }
    break;
    case RendererCommandType::eSetVertexShader:
    {
      const vdl::VertexShader& VertexShader = VertexShaders_[RendererCommand.second];

      _pDeviceContext->VSSetShader(VertexShader);
    }
    break;
    case RendererCommandType::eSetHullShader:
    {
      const vdl::HullShader& HullShader = HullShaders_[RendererCommand.second];

      _pDeviceContext->HSSetShader(HullShader);
    }
    break;
    case RendererCommandType::eSetDomainShader:
    {
      const vdl::DomainShader& DomainShader = DomainShaders_[RendererCommand.second];

      _pDeviceContext->DSSetShader(DomainShader);
    }
    break;
    case RendererCommandType::eSetGeometryShader:
    {
      const vdl::GeometryShader& GeometryShader = GeometryShaders_[RendererCommand.second];

      _pDeviceContext->GSSetShader(GeometryShader);
    }
    break;
    case RendererCommandType::eSetPixelShader:
    {
      const vdl::PixelShader& PixelShader = PixelShaders_[RendererCommand.second];

      _pDeviceContext->PSSetShader(PixelShader);
    }
    break;
    case RendererCommandType::eSetVertexStageSampler:
    {
      const Samplers& Samplers = GetSamplers(RendererCommand.second, ShaderType::eVertexShader);

      _pDeviceContext->VSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
    }
    break;
    case RendererCommandType::eSetHullStageSampler:
    {
      const Samplers& Samplers = GetSamplers(RendererCommand.second, ShaderType::eHullShader);

      _pDeviceContext->HSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
    }
    break;
    case RendererCommandType::eSetDomainStageSampler:
    {
      const Samplers& Samplers = GetSamplers(RendererCommand.second, ShaderType::eDomainShader);

      _pDeviceContext->DSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
    }
    break;
    case RendererCommandType::eSetGeometryStageSampler:
    {
      const Samplers& Samplers = GetSamplers(RendererCommand.second, ShaderType::eGeometryShader);

      _pDeviceContext->GSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
    }
    break;
    case RendererCommandType::eSetPixelStageSampler:
    {
      const Samplers& Samplers = GetSamplers(RendererCommand.second, ShaderType::ePixelShader);

      _pDeviceContext->PSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
    }
    break;
    case RendererCommandType::eSetVertexStageTexture:
    {
      const std::vector<vdl::Texture> Textures = std::move(GetTextures(RendererCommand.second, ShaderType::eVertexShader));

      _pDeviceContext->VSSetTextures(0, static_cast<vdl::uint>(Textures.size()), Textures.data());
    }
    break;
    case RendererCommandType::eSetHullStageTexture:
    {
      const std::vector<vdl::Texture> Textures = std::move(GetTextures(RendererCommand.second, ShaderType::eHullShader));

      _pDeviceContext->HSSetTextures(0, static_cast<vdl::uint>(Textures.size()), Textures.data());
    }
    break;
    case RendererCommandType::eSetDomainStageTexture:
    {
      const std::vector<vdl::Texture> Textures = std::move(GetTextures(RendererCommand.second, ShaderType::eDomainShader));

      _pDeviceContext->DSSetTextures(0, static_cast<vdl::uint>(Textures.size()), Textures.data());
    }
    break;
    case RendererCommandType::eSetGeometryStageTexture:
    {
      const std::vector<vdl::Texture> Textures = std::move(GetTextures(RendererCommand.second, ShaderType::eGeometryShader));

      _pDeviceContext->GSSetTextures(0, static_cast<vdl::uint>(Textures.size()), Textures.data());
    }
    break;
    case RendererCommandType::eSetPixelStageTexture:
    {
      const std::vector<vdl::Texture> Textures = std::move(GetTextures(RendererCommand.second, ShaderType::ePixelShader));

      _pDeviceContext->PSSetTextures(0, static_cast<vdl::uint>(Textures.size()), Textures.data());
    }
    break;
    case RendererCommandType::eSetVertexStageConstantBuffer:
    {
      const std::vector<vdl::Detail::ConstantBufferData> ConstantBuffers = std::move(GetConstantBuffers(RendererCommand.second, ShaderType::eVertexShader));

      _pDeviceContext->VSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
    }
    break;
    case RendererCommandType::eSetHullStageConstantBuffer:
    {
      const std::vector<vdl::Detail::ConstantBufferData> ConstantBuffers = std::move(GetConstantBuffers(RendererCommand.second, ShaderType::eHullShader));

      _pDeviceContext->HSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
    }
    break;
    case RendererCommandType::eSetDomainStageConstantBuffer:
    {
      const std::vector<vdl::Detail::ConstantBufferData> ConstantBuffers = std::move(GetConstantBuffers(RendererCommand.second, ShaderType::eDomainShader));

      _pDeviceContext->DSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
    }
    break;
    case RendererCommandType::eSetGeometryStageConstantBuffer:
    {
      const std::vector<vdl::Detail::ConstantBufferData> ConstantBuffers = std::move(GetConstantBuffers(RendererCommand.second, ShaderType::eGeometryShader));

      _pDeviceContext->GSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
    }
    break;
    case RendererCommandType::eSetPixelStageConstantBuffer:
    {
      const std::vector<vdl::Detail::ConstantBufferData> ConstantBuffers = std::move(GetConstantBuffers(RendererCommand.second, ShaderType::ePixelShader));

      _pDeviceContext->PSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
    }
    break;
    default: assert(false);
    }
  }
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushDrawData(const DisplayObject& _DisplayObject, InstanceData&& _InstanceData)
{
  if (StateChangeFlags_.Has(RendererCommandType::eSetScissor))
  {
    RendererCommands_.emplace_back(RendererCommandType::eSetScissor, static_cast<vdl::uint>(Scissors_.size()));
    Scissors_.push_back(CurrentScissor_);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetViewport))
  {
    RendererCommands_.emplace_back(RendererCommandType::eSetViewport, static_cast<vdl::uint>(Viewports_.size()));
    Viewports_.push_back(CurrentViewport_);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetBlendState))
  {
    RendererCommands_.emplace_back(RendererCommandType::eSetBlendState, static_cast<vdl::uint>(BlendStates_.size()));
    BlendStates_.push_back(CurrentBlendState_);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetDepthStencilState))
  {
    RendererCommands_.emplace_back(RendererCommandType::eSetDepthStencilState, static_cast<vdl::uint>(DepthStencilStates_.size()));
    DepthStencilStates_.push_back(CurrentDepthStencilState_);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetRasterizerState))
  {
    RendererCommands_.emplace_back(RendererCommandType::eSetRasterizerState, static_cast<vdl::uint>(RasterizerStates_.size()));
    RasterizerStates_.push_back(CurrentRasterizerState_);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetVertexShader))
  {
    RendererCommands_.emplace_back(RendererCommandType::eSetVertexShader, static_cast<vdl::uint>(VertexShaders_.size()));
    VertexShaders_.push_back(CurrentVertexShader_);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetHullShader))
  {
    RendererCommands_.emplace_back(RendererCommandType::eSetHullShader, static_cast<vdl::uint>(HullShaders_.size()));
    HullShaders_.push_back(CurrentHullShader_);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetDomainShader))
  {
    RendererCommands_.emplace_back(RendererCommandType::eSetDomainShader, static_cast<vdl::uint>(DomainShaders_.size()));
    DomainShaders_.push_back(CurrentDomainShader_);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetGeometryShader))
  {
    RendererCommands_.emplace_back(RendererCommandType::eSetGeometryShader, static_cast<vdl::uint>(GeometryShaders_.size()));
    GeometryShaders_.push_back(CurrentGeometryShader_);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetPixelShader))
  {
    RendererCommands_.emplace_back(RendererCommandType::eSetPixelShader, static_cast<vdl::uint>(PixelShaders_.size()));
    PixelShaders_.push_back(CurrentPixelShader_);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetVertexStageSampler))
  {
    auto& Sampler = Samplers_[static_cast<vdl::uint>(ShaderType::eVertexShader)];

    RendererCommands_.emplace_back(RendererCommandType::eSetVertexStageSampler, static_cast<vdl::uint>(Sampler.size()));
    Sampler.push_back(CurrentSamplers_[static_cast<vdl::uint>(ShaderType::eVertexShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetHullStageSampler))
  {
    auto& Sampler = Samplers_[static_cast<vdl::uint>(ShaderType::eHullShader)];

    RendererCommands_.emplace_back(RendererCommandType::eSetHullStageSampler, static_cast<vdl::uint>(Sampler.size()));
    Sampler.push_back(CurrentSamplers_[static_cast<vdl::uint>(ShaderType::eHullShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetDomainStageSampler))
  {
    auto& Sampler = Samplers_[static_cast<vdl::uint>(ShaderType::eDomainShader)];

    RendererCommands_.emplace_back(RendererCommandType::eSetDomainStageSampler, static_cast<vdl::uint>(Sampler.size()));
    Sampler.push_back(CurrentSamplers_[static_cast<vdl::uint>(ShaderType::eDomainShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetGeometryStageSampler))
  {
    auto& Sampler = Samplers_[static_cast<vdl::uint>(ShaderType::eGeometryShader)];

    RendererCommands_.emplace_back(RendererCommandType::eSetGeometryStageSampler, static_cast<vdl::uint>(Sampler.size()));
    Sampler.push_back(CurrentSamplers_[static_cast<vdl::uint>(ShaderType::eGeometryShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetPixelStageSampler))
  {
    auto& Sampler = Samplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)];

    RendererCommands_.emplace_back(RendererCommandType::eSetPixelStageSampler, static_cast<vdl::uint>(Sampler.size()));
    Sampler.push_back(CurrentSamplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetVertexStageTexture))
  {
    auto& TextureIDs = TextureIDs_[static_cast<vdl::uint>(ShaderType::eVertexShader)];

    RendererCommands_.emplace_back(RendererCommandType::eSetVertexStageTexture, static_cast<vdl::uint>(TextureIDs.size()));
    TextureIDs.push_back(CurrentTextureIDs_[static_cast<vdl::uint>(ShaderType::eVertexShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetHullStageTexture))
  {
    auto& TextureIDs = TextureIDs_[static_cast<vdl::uint>(ShaderType::eHullShader)];

    RendererCommands_.emplace_back(RendererCommandType::eSetHullStageTexture, static_cast<vdl::uint>(TextureIDs.size()));
    TextureIDs.push_back(CurrentTextureIDs_[static_cast<vdl::uint>(ShaderType::eHullShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetDomainStageTexture))
  {
    auto& TextureIDs = TextureIDs_[static_cast<vdl::uint>(ShaderType::eDomainShader)];

    RendererCommands_.emplace_back(RendererCommandType::eSetDomainStageTexture, static_cast<vdl::uint>(TextureIDs.size()));
    TextureIDs.push_back(CurrentTextureIDs_[static_cast<vdl::uint>(ShaderType::eDomainShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetGeometryStageTexture))
  {
    auto& TextureIDs = TextureIDs_[static_cast<vdl::uint>(ShaderType::eGeometryShader)];

    RendererCommands_.emplace_back(RendererCommandType::eSetGeometryStageTexture, static_cast<vdl::uint>(TextureIDs.size()));
    TextureIDs.push_back(CurrentTextureIDs_[static_cast<vdl::uint>(ShaderType::eGeometryShader)]);
  }

  if (StateChangeFlags_.Has(RendererCommandType::eSetPixelStageTexture))
  {
    auto& TextureIDs = TextureIDs_[static_cast<vdl::uint>(ShaderType::ePixelShader)];

    RendererCommands_.emplace_back(RendererCommandType::eSetPixelStageTexture, static_cast<vdl::uint>(TextureIDs.size()));
    TextureIDs.push_back(CurrentTextureIDs_[static_cast<vdl::uint>(ShaderType::ePixelShader)]);
  }

  //  SetConstantBuffer
  {
    auto IssueRendererCommandSetConstantBuffer = [this](RendererCommandType _RendererCommandType, ShaderType _ShaderType)->void
    {
      ConstantBufferIDs TempConstantBufferIDs;
      auto& CurrentConstantBufferIDs = CurrentConstantBufferIDs_[static_cast<vdl::uint>(_ShaderType)];
      auto& ConstantBufferIDs = ConstantBufferIDs_[static_cast<vdl::uint>(_ShaderType)];
      auto& LastConstantBufferIDs = LastConstantBufferIDs_[static_cast<vdl::uint>(_ShaderType)];

      TempConstantBufferIDs = ConstantBufferIDs.back();
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

            StateChangeFlags_.Set(_RendererCommandType);
          }

          ++ConstantBufferCount;
        }
        while (ConstantBufferCount < CurrentConstantBufferNum)
        {
          vdl::Detail::ConstantBufferData ConstantBuffer = std::move(pBufferManager_->CloneConstantBuffer(ReservedConstantBuffers_.at(CurrentConstantBufferIDs[ConstantBufferCount])));
          const vdl::ID ConstantBufferID = ConstantBuffer.GetID();

          ReservedConstantBuffers_.insert(std::make_pair(ConstantBufferID, std::move(ConstantBuffer)));
          TempConstantBufferIDs[ConstantBufferCount] = ConstantBufferID;

          StateChangeFlags_.Set(_RendererCommandType);

          ++ConstantBufferCount;
        }
      }

      if (StateChangeFlags_.Has(_RendererCommandType))
      {
        RendererCommands_.emplace_back(_RendererCommandType, static_cast<vdl::uint>(ConstantBufferIDs.size()));
        LastConstantBufferIDs = CurrentConstantBufferIDs;
        ConstantBufferIDs.push_back(std::move(TempConstantBufferIDs));
      }
    };

    IssueRendererCommandSetConstantBuffer(RendererCommandType::eSetVertexStageConstantBuffer, ShaderType::eVertexShader);

    IssueRendererCommandSetConstantBuffer(RendererCommandType::eSetHullStageConstantBuffer, ShaderType::eHullShader);

    IssueRendererCommandSetConstantBuffer(RendererCommandType::eSetDomainStageConstantBuffer, ShaderType::eDomainShader);

    IssueRendererCommandSetConstantBuffer(RendererCommandType::eSetGeometryStageConstantBuffer, ShaderType::eGeometryShader);

    IssueRendererCommandSetConstantBuffer(RendererCommandType::eSetPixelStageConstantBuffer, ShaderType::ePixelShader);
  }

  //  Draw
  {
    const vdl::ID DiplayObjectID = _DisplayObject.GetID();

    if (ReservedDisplayObjects_.find(DiplayObjectID) == ReservedDisplayObjects_.end())
    {
      ReservedDisplayObjects_.insert(std::make_pair(DiplayObjectID, _DisplayObject));
    }

    RendererCommands_.emplace_back(RendererCommandType::eDraw, static_cast<vdl::uint>(DisplayObjectIDs_.size()));
    DisplayObjectIDs_.push_back(DiplayObjectID);
    Instances_.emplace_back(_InstanceData);
  }

  StateChangeFlags_.Clear();
}

#define SetState(RendererCommandType, StateName)\
if (StateChangeFlags_.Has(RendererCommandType))\
{\
  Current##StateName##_ = _##StateName;\
  \
  if (Current##StateName##_ == StateName##s_.back())\
  {\
    StateChangeFlags_.Cancel(RendererCommandType);\
  }\
}\
else\
{\
  if (Current##StateName##_ != _##StateName)\
  {\
    Current##StateName##_ = _##StateName;\
    StateChangeFlags_.Set(RendererCommandType);\
  }\
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushScissor(const vdl::Scissor& _Scissor)
{
  SetState(RendererCommandType::eSetScissor, Scissor)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushViewport(const vdl::Viewport& _Viewport)
{
  SetState(RendererCommandType::eSetViewport, Viewport)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushBlendState(const vdl::BlendState& _BlendState)
{
  SetState(RendererCommandType::eSetBlendState, BlendState)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushDepthStencilState(const vdl::DepthStencilState& _DepthStencilState)
{
  SetState(RendererCommandType::eSetDepthStencilState, DepthStencilState)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushRasterizerState(const vdl::RasterizerState& _RasterizerState)
{
  SetState(RendererCommandType::eSetRasterizerState, RasterizerState)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushVertexShader(const vdl::VertexShader& _VertexShader)
{
  SetState(RendererCommandType::eSetVertexShader, VertexShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushHullShader(const vdl::HullShader& _HullShader)
{
  SetState(RendererCommandType::eSetHullShader, HullShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushDomainShader(const vdl::DomainShader& _DomainShader)
{
  SetState(RendererCommandType::eSetDomainShader, DomainShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushGeometryShader(const vdl::GeometryShader& _GeometryShader)
{
  SetState(RendererCommandType::eSetGeometryShader, GeometryShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushPixelShader(const vdl::PixelShader& _PixelShader)
{
  SetState(RendererCommandType::eSetPixelShader, PixelShader)
}

#undef SetState

#define SetSampler(RendererCommandType, ShaderType)\
Samplers& CurrentSamplers = CurrentSamplers_[static_cast<vdl::uint>(ShaderType)];\
\
if (CurrentSamplers.size() <= _Slot)\
{\
  CurrentSamplers.resize(_Slot + 1);\
  StateChangeFlags_.Set(RendererCommandType);\
}\
\
if (StateChangeFlags_.Has(RendererCommandType))\
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
  for (vdl::uint SamplerCount = 0; SamplerCount < static_cast<vdl::uint>(CurrentSamplerNum); ++SamplerCount)\
  {\
    if (CurrentSamplers[SamplerCount] != LastSamplers[SamplerCount])\
    {\
      return;\
    }\
  }\
  \
  StateChangeFlags_.Cancel(RendererCommandType);\
}\
else\
{\
  if (CurrentSamplers[_Slot] != _Sampler)\
  {\
    CurrentSamplers[_Slot] = _Sampler;\
    StateChangeFlags_.Set(RendererCommandType);\
  }\
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushVertexStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot)
{
  SetSampler(RendererCommandType::eSetVertexStageSampler, ShaderType::eVertexShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushHullStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot)
{
  SetSampler(RendererCommandType::eSetHullStageSampler, ShaderType::eHullShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushDomainStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot)
{
  SetSampler(RendererCommandType::eSetDomainStageSampler, ShaderType::eDomainShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushGeometryStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot)
{
  SetSampler(RendererCommandType::eSetGeometryStageSampler, ShaderType::eGeometryShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushPixelStageSampler(const vdl::Sampler& _Sampler, vdl::uint _Slot)
{
  SetSampler(RendererCommandType::eSetPixelStageSampler, ShaderType::ePixelShader)
}

#undef SetSampler

#define SetTexture(RendererCommandType, ShaderType)\
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
  StateChangeFlags_.Set(RendererCommandType);\
}\
\
if (StateChangeFlags_.Has(RendererCommandType))\
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
  StateChangeFlags_.Cancel(RendererCommandType);\
}\
else\
{\
  if (CurrentTextureIDs[_Slot] != TextureID)\
  {\
    CurrentTextureIDs[_Slot] = TextureID;\
    StateChangeFlags_.Set(RendererCommandType);\
  }\
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushVertexStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot)
{
  SetTexture(RendererCommandType::eSetVertexStageTexture, ShaderType::eVertexShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushHullStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot)
{
  SetTexture(RendererCommandType::eSetHullStageTexture, ShaderType::eHullShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushDomainStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot)
{
  SetTexture(RendererCommandType::eSetDomainStageTexture, ShaderType::eDomainShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushGeometryStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot)
{
  SetTexture(RendererCommandType::eSetGeometryStageTexture, ShaderType::eGeometryShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushPixelStageTexture(const vdl::Texture& _Texture, vdl::uint _Slot)
{
  SetTexture(RendererCommandType::eSetPixelStageTexture, ShaderType::ePixelShader)
}

#undef SetTexture

#define SetConstantBuffer(RendererCommandType, ShaderType)\
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
  StateChangeFlags_.Set(RendererCommandType);\
}\
\
if (StateChangeFlags_.Has(RendererCommandType))\
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
  StateChangeFlags_.Cancel(RendererCommandType);\
}\
else\
{\
  if (CurrentConstantBufferIDs[_Slot] != ConstantBufferID)\
  {\
    CurrentConstantBufferIDs[_Slot] = ConstantBufferID;\
    StateChangeFlags_.Set(RendererCommandType);\
  }\
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushVertexStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot)
{
  SetConstantBuffer(RendererCommandType::eSetVertexStageConstantBuffer, ShaderType::eVertexShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushHullStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot)
{
  SetConstantBuffer(RendererCommandType::eSetHullStageConstantBuffer, ShaderType::eHullShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushDomainStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot)
{
  SetConstantBuffer(RendererCommandType::eSetDomainStageConstantBuffer, ShaderType::eDomainShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushGeometryStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot)
{
  SetConstantBuffer(RendererCommandType::eSetGeometryStageConstantBuffer, ShaderType::eGeometryShader)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushPixelStageConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer, vdl::uint _Slot)
{
  SetConstantBuffer(RendererCommandType::eSetPixelStageConstantBuffer, ShaderType::ePixelShader)
}

#undef SetConstantBuffer
