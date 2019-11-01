#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>
#include <vdl/ModelManager/IModelManager.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>

#include <vdl/Model.hpp>

#include <vdl/Math.hpp>

#include <algorithm>

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::Initialize(vdl::TopologyType _Topology, const vdl::BlendState& _BlendState, const vdl::DepthStencilState& _DepthStencilState, const vdl::RasterizerState& _RasterizerState,
  const vdl::Sampler& _Sampler, const vdl::VertexShader& _VertexShader, const vdl::PixelShader& _PixelShader)
{
  pDevice_ = Engine::Get<IDevice>();
  if constexpr (std::is_same<DisplayObject, vdl::Mesh>::value)
  {
    pModelManager_ = Engine::Get<IModelManager>();
  }
  pBufferManager_ = Engine::Get<IBufferManager>();

  Topologys_.push_back(CurrentTopology_ = _Topology);
  Scissors_.push_back(CurrentScissor_ = vdl::Scissor());
  Viewports_.push_back(CurrentViewport_ = vdl::Viewport());
  BlendStates_.push_back(CurrentBlendState_ = _BlendState);
  DepthStencilStates_.push_back(CurrentDepthStencilState_ = _DepthStencilState);
  RasterizerStates_.push_back(CurrentRasterizerState_ = _RasterizerState);
  VertexShaders_.push_back(CurrentVertexShader_ = _VertexShader);
  PixelShaders_.push_back(CurrentPixelShader_ = _PixelShader);
  HullShaders_.push_back(CurrentHullShader_ = vdl::HullShader());
  DomainShaders_.push_back(CurrentDomainShader_ = vdl::DomainShader());
  GeometryShaders_.push_back(CurrentGeometryShader_ = vdl::GeometryShader());

  for (auto& ShaderResources : ShaderResources_) { ShaderResources.resize(1); }
  for (auto& Samplers : Samplers_) { Samplers.resize(1); }
  for (auto& ConstantBuffers : ConstantBuffers_) { ConstantBuffers.resize(1); }

  CurrentSamplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)].resize(1);
  Samplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][0].push_back(CurrentSamplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][0] = _Sampler);

  Reset();
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::Reset()
{
  RendererCommands_.clear();
  StateChangeFlags_.Clear();
  Instances_.clear();
  DisplayObjectIDs_.clear();

  Topologys_ = { std::move(Topologys_.back()) };
  StateChangeFlags_.Set(RendererCommandType::eSetTopology);

  Scissors_ = { std::move(Scissors_.back()) };
  StateChangeFlags_.Set(RendererCommandType::eSetScissor);

  Viewports_ = { std::move(Viewports_.back()) };
  StateChangeFlags_.Set(RendererCommandType::eSetViewport);

  BlendStates_ = { std::move(BlendStates_.back()) };
  StateChangeFlags_.Set(RendererCommandType::eSetBlendState);

  DepthStencilStates_ = { std::move(DepthStencilStates_.back()) };
  StateChangeFlags_.Set(RendererCommandType::eSetDepthStencilState);

  RasterizerStates_ = { std::move(RasterizerStates_.back()) };
  StateChangeFlags_.Set(RendererCommandType::eSetRasterizerState);

  VertexShaders_ = { std::move(VertexShaders_.back()) };
  StateChangeFlags_.Set(RendererCommandType::eSetVertexShader);

  HullShaders_ = { std::move(HullShaders_.back()) };
  StateChangeFlags_.Set(RendererCommandType::eSetHullShader);

  DomainShaders_ = { std::move(DomainShaders_.back()) };
  StateChangeFlags_.Set(RendererCommandType::eSetDomainShader);

  GeometryShaders_ = { std::move(GeometryShaders_.back()) };
  StateChangeFlags_.Set(RendererCommandType::eSetGeometryShader);

  PixelShaders_ = { std::move(PixelShaders_.back()) };
  StateChangeFlags_.Set(RendererCommandType::eSetPixelShader);

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  {
    auto& ShaderResources = ShaderResources_[ShaderTypeCount];

    ShaderResources = { std::move(ShaderResources.back()) };
    StateChangeFlags_.Set(static_cast<RendererCommandType>(static_cast<vdl::uint>(RendererCommandType::eSetVertexStageShaderResource) + ShaderTypeCount));
  }

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  {
    auto& Samplers = Samplers_[ShaderTypeCount];

    Samplers = { std::move(Samplers.back()) };
    StateChangeFlags_.Set(static_cast<RendererCommandType>(static_cast<vdl::uint>(RendererCommandType::eSetVertexStageSampler) + ShaderTypeCount));
  }

  for (vdl::uint ShaderTypeCount = 0; ShaderTypeCount < kGraphicsShaderStageNum; ++ShaderTypeCount)
  {
    auto& ConstantBuffers = ConstantBuffers_[ShaderTypeCount];

    ConstantBuffers = { std::move(ConstantBuffers.back()) };
    StateChangeFlags_.Set(static_cast<RendererCommandType>(static_cast<vdl::uint>(RendererCommandType::eSetVertexStageConstantBuffer) + ShaderTypeCount));
  }

  ReservedDisplayObjects_.clear();
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::Adjust()
{
  if constexpr (!std::is_same<DisplayObject, Empty>::value)
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
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::Flush(IDeviceContext* _pDeviceContext, const InstanceBuffer& _InstanceBuffer)
{
  if constexpr (!std::is_same<DisplayObject, Empty>::value)
  {
    //  ソートが終わるまで待機
    Thread_.join();
  }

  const vdl::uint RendererCommandNum = static_cast<vdl::uint>(RendererCommands_.size());
  for (vdl::uint RendererCommandCount = 0; RendererCommandCount < RendererCommandNum; ++RendererCommandCount)
  {
    const RendererCommandPair& RendererCommand = RendererCommands_[RendererCommandCount];

    switch (RendererCommand.first)
    {
    case RendererCommandType::eDraw:
    {
      if constexpr (std::is_same<DisplayObject, Empty>::value)
      {
        _pDeviceContext->SetVertexBuffer(VertexBuffer());
        _pDeviceContext->Draw(Instances_[RendererCommand.second], 1, 0, 0);
      }
      else
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
          DrawState = (++ContinuousDrawCallNum >= kMaxBatchNum ? DrawStateType::eFlush : DrawStateType::eContinue);

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

            _pDeviceContext->SetInstanceBuffer(_InstanceBuffer);

            if constexpr (std::is_same<DisplayObject, vdl::Texture>::value)
            {
              pDevice_->WriteMemory(pBufferManager_->GetBuffer(_InstanceBuffer.GetID()), Instances.data(), sizeof(InstanceData) * ContinuousDrawCallNum);

              _pDeviceContext->PSSetShaderResources(0, 1, &vdl::ShaderResource(ReservedDisplayObjects_[CurrentDisplayObjectID]));

              _pDeviceContext->Draw(4, ContinuousDrawCallNum, 0, 0);
            }
            else if constexpr (std::is_same<DisplayObject, vdl::Mesh>::value)
            {
              const Mesh* pMesh = pModelManager_->GetMesh(CurrentDisplayObjectID);

              _pDeviceContext->SetVertexBuffer(pMesh->VertexBuffer);
              _pDeviceContext->SetIndexBuffer(pMesh->IndexBuffer);

              std::vector<InstanceData> TempInstances = Instances;
              for (auto& TempInstance : TempInstances)
              {
                TempInstance.Color.Red *= pMesh->Material.MaterialColor.Red;
                TempInstance.Color.Green *= pMesh->Material.MaterialColor.Green;
                TempInstance.Color.Blue *= pMesh->Material.MaterialColor.Blue;
                TempInstance.Color.Alpha *= pMesh->Material.MaterialColor.Alpha;
              }

              pDevice_->WriteMemory(pBufferManager_->GetBuffer(_InstanceBuffer.GetID()), TempInstances.data(), sizeof(InstanceData) * ContinuousDrawCallNum);

              _pDeviceContext->PSSetShaderResources(0, 1, &vdl::ShaderResource(pMesh->Material.Diffuse));
              _pDeviceContext->PSSetShaderResources(1, 1, &vdl::ShaderResource(pMesh->Material.NormalMap));

              _pDeviceContext->DrawIndexed(pMesh->IndexCount, ContinuousDrawCallNum, pMesh->IndexStart, 0, 0);
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
    }
    break;
    case RendererCommandType::eSetTopology:
    {
      const vdl::TopologyType& Topology = Topologys_[RendererCommand.second];

      _pDeviceContext->SetTopology(Topology);
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
    case RendererCommandType::eSetVertexStageShaderResource:
    {
      const ShaderResources& ShaderResources = ShaderResources_[static_cast<vdl::uint>(ShaderType::eVertexShader)][RendererCommand.second];

      _pDeviceContext->VSSetShaderResources(0, static_cast<vdl::uint>(ShaderResources.size()), ShaderResources.data());
    }
    break;
    case RendererCommandType::eSetHullStageShaderResource:
    {
      const ShaderResources& ShaderResources = ShaderResources_[static_cast<vdl::uint>(ShaderType::eHullShader)][RendererCommand.second];

      _pDeviceContext->HSSetShaderResources(0, static_cast<vdl::uint>(ShaderResources.size()), ShaderResources.data());
    }
    break;
    case RendererCommandType::eSetDomainStageShaderResource:
    {
      const ShaderResources& ShaderResources = ShaderResources_[static_cast<vdl::uint>(ShaderType::eDomainShader)][RendererCommand.second];

      _pDeviceContext->DSSetShaderResources(0, static_cast<vdl::uint>(ShaderResources.size()), ShaderResources.data());
    }
    break;
    case RendererCommandType::eSetGeometryStageShaderResource:
    {
      const ShaderResources& ShaderResources = ShaderResources_[static_cast<vdl::uint>(ShaderType::eGeometryShader)][RendererCommand.second];

      _pDeviceContext->GSSetShaderResources(0, static_cast<vdl::uint>(ShaderResources.size()), ShaderResources.data());
    }
    break;
    case RendererCommandType::eSetPixelStageShaderResource:
    {
      const ShaderResources& ShaderResources = ShaderResources_[static_cast<vdl::uint>(ShaderType::ePixelShader)][RendererCommand.second];

      _pDeviceContext->PSSetShaderResources(0, static_cast<vdl::uint>(ShaderResources.size()), ShaderResources.data());
    }
    break;
    case RendererCommandType::eSetVertexStageSampler:
    {
      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(ShaderType::eVertexShader)][RendererCommand.second];

      _pDeviceContext->VSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
    }
    break;
    case RendererCommandType::eSetHullStageSampler:
    {
      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(ShaderType::eHullShader)][RendererCommand.second];

      _pDeviceContext->HSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
    }
    break;
    case RendererCommandType::eSetDomainStageSampler:
    {
      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(ShaderType::eDomainShader)][RendererCommand.second];

      _pDeviceContext->DSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
    }
    break;
    case RendererCommandType::eSetGeometryStageSampler:
    {
      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(ShaderType::eGeometryShader)][RendererCommand.second];

      _pDeviceContext->GSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
    }
    break;
    case RendererCommandType::eSetPixelStageSampler:
    {
      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][RendererCommand.second];

      _pDeviceContext->PSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
    }
    break;
    case RendererCommandType::eSetVertexStageConstantBuffer:
    {
      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(ShaderType::eVertexShader)][RendererCommand.second];

      _pDeviceContext->VSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
    }
    break;
    case RendererCommandType::eSetHullStageConstantBuffer:
    {
      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(ShaderType::eHullShader)][RendererCommand.second];

      _pDeviceContext->HSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
    }
    break;
    case RendererCommandType::eSetDomainStageConstantBuffer:
    {
      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(ShaderType::eDomainShader)][RendererCommand.second];

      _pDeviceContext->DSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
    }
    break;
    case RendererCommandType::eSetGeometryStageConstantBuffer:
    {
      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(ShaderType::eGeometryShader)][RendererCommand.second];

      _pDeviceContext->GSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
    }
    break;
    case RendererCommandType::eSetPixelStageConstantBuffer:
    {
      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(ShaderType::ePixelShader)][RendererCommand.second];

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
  //  SetConstantBuffer
  {
    auto IssueRendererCommandSetConstantBuffer = [this](RendererCommandType _RendererCommandType, ShaderType _ShaderType)->void
    {
      ConstantBuffers TempConstantBuffers;
      auto& CurrentConstantBuffers = CurrentConstantBuffers_[static_cast<vdl::uint>(_ShaderType)];
      auto& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(_ShaderType)];
      auto& LastConstantBuffers = LastConstantBuffers_[static_cast<vdl::uint>(_ShaderType)];

      TempConstantBuffers = ConstantBuffers.back();
      {
        const size_t CurrentConstantBufferNum = CurrentConstantBuffers.size();
        const size_t LastConstantBufferNum = LastConstantBuffers.size();

        if (TempConstantBuffers.size() < CurrentConstantBufferNum)
        {
          TempConstantBuffers.resize(CurrentConstantBufferNum);
          StateChangeFlags_.Set(_RendererCommandType);
        }

        size_t ConstantBufferCount = 0;
        while (ConstantBufferCount < LastConstantBufferNum)
        {
          const vdl::Detail::ConstantBufferData& CurrentConstantBuffer = CurrentConstantBuffers[ConstantBufferCount];
          const vdl::Detail::ConstantBufferData& LastConstantBuffer = LastConstantBuffers[ConstantBufferCount];
          const vdl::Detail::ConstantBufferData& BeforeConstantBuffer = TempConstantBuffers[ConstantBufferCount];

          if (CurrentConstantBuffer != LastConstantBuffer
            || ::memcmp(CurrentConstantBuffer.GetData(), BeforeConstantBuffer.GetData(), CurrentConstantBuffer.GetBufferSize()) != 0)
          {
            TempConstantBuffers[ConstantBufferCount] = pBufferManager_->CloneConstantBuffer(CurrentConstantBuffer);
            StateChangeFlags_.Set(_RendererCommandType);
          }

          ++ConstantBufferCount;
        }
        while (ConstantBufferCount < CurrentConstantBufferNum)
        {
          TempConstantBuffers[ConstantBufferCount] = pBufferManager_->CloneConstantBuffer(CurrentConstantBuffers[ConstantBufferCount]);
          StateChangeFlags_.Set(_RendererCommandType);

          ++ConstantBufferCount;
        }
      }

      if (StateChangeFlags_.Has(_RendererCommandType))
      {
        RendererCommands_.emplace_back(_RendererCommandType, static_cast<vdl::uint>(ConstantBuffers.size()));
        LastConstantBuffers = CurrentConstantBuffers;
        ConstantBuffers.emplace_back(std::move(TempConstantBuffers));
        StateChangeFlags_.Cancel(_RendererCommandType);
      }
    };

    IssueRendererCommandSetConstantBuffer(RendererCommandType::eSetVertexStageConstantBuffer, ShaderType::eVertexShader);

    IssueRendererCommandSetConstantBuffer(RendererCommandType::eSetHullStageConstantBuffer, ShaderType::eHullShader);

    IssueRendererCommandSetConstantBuffer(RendererCommandType::eSetDomainStageConstantBuffer, ShaderType::eDomainShader);

    IssueRendererCommandSetConstantBuffer(RendererCommandType::eSetGeometryStageConstantBuffer, ShaderType::eGeometryShader);

    IssueRendererCommandSetConstantBuffer(RendererCommandType::eSetPixelStageConstantBuffer, ShaderType::ePixelShader);
  }

  if (!StateChangeFlags_.isEmpty())
  {
    if (StateChangeFlags_.Has(RendererCommandType::eSetTopology))
    {
      RendererCommands_.emplace_back(RendererCommandType::eSetTopology, static_cast<vdl::uint>(Topologys_.size()));
      Topologys_.push_back(CurrentTopology_);
    }

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

    if (StateChangeFlags_.Has(RendererCommandType::eSetVertexStageShaderResource))
    {
      auto& ShaderResources = ShaderResources_[static_cast<vdl::uint>(ShaderType::eVertexShader)];

      RendererCommands_.emplace_back(RendererCommandType::eSetVertexStageShaderResource, static_cast<vdl::uint>(ShaderResources.size()));
      ShaderResources.push_back(CurrentShaderResources_[static_cast<vdl::uint>(ShaderType::eVertexShader)]);
    }

    if (StateChangeFlags_.Has(RendererCommandType::eSetHullStageShaderResource))
    {
      auto& ShaderResources = ShaderResources_[static_cast<vdl::uint>(ShaderType::eHullShader)];

      RendererCommands_.emplace_back(RendererCommandType::eSetHullStageShaderResource, static_cast<vdl::uint>(ShaderResources.size()));
      ShaderResources.push_back(CurrentShaderResources_[static_cast<vdl::uint>(ShaderType::eHullShader)]);
    }

    if (StateChangeFlags_.Has(RendererCommandType::eSetDomainStageShaderResource))
    {
      auto& ShaderResources = ShaderResources_[static_cast<vdl::uint>(ShaderType::eDomainShader)];

      RendererCommands_.emplace_back(RendererCommandType::eSetDomainStageShaderResource, static_cast<vdl::uint>(ShaderResources.size()));
      ShaderResources.push_back(CurrentShaderResources_[static_cast<vdl::uint>(ShaderType::eDomainShader)]);
    }

    if (StateChangeFlags_.Has(RendererCommandType::eSetGeometryStageShaderResource))
    {
      auto& ShaderResources = ShaderResources_[static_cast<vdl::uint>(ShaderType::eGeometryShader)];

      RendererCommands_.emplace_back(RendererCommandType::eSetGeometryStageShaderResource, static_cast<vdl::uint>(ShaderResources.size()));
      ShaderResources.push_back(CurrentShaderResources_[static_cast<vdl::uint>(ShaderType::eGeometryShader)]);
    }

    if (StateChangeFlags_.Has(RendererCommandType::eSetPixelStageShaderResource))
    {
      auto& ShaderResources = ShaderResources_[static_cast<vdl::uint>(ShaderType::ePixelShader)];

      RendererCommands_.emplace_back(RendererCommandType::eSetPixelStageShaderResource, static_cast<vdl::uint>(ShaderResources.size()));
      ShaderResources.push_back(CurrentShaderResources_[static_cast<vdl::uint>(ShaderType::ePixelShader)]);
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
  }

  StateChangeFlags_.Clear();

  //  Draw
  {
    if constexpr (std::is_same<DisplayObject, Empty>::value)
    {
      RendererCommands_.emplace_back(RendererCommandType::eDraw, static_cast<vdl::uint>(DisplayObjectIDs_.size()));
      Instances_.emplace_back(_InstanceData);
    }
    else
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
  }
}

#define SetState(RendererCommandType, StateName)\
if (!StateChangeFlags_.Has(RendererCommandType) && Current##StateName##_ != _##StateName)\
{\
  StateChangeFlags_.Set(RendererCommandType);\
}\
\
Current##StateName##_ = _##StateName;\

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushTopology(const vdl::TopologyType& _Topology)
{
  SetState(RendererCommandType::eSetTopology, Topology)
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

#define SetShaderStates(RendererCommandType, ShaderType, ShaderState)\
ShaderState##s& Current##ShaderState##s = Current##ShaderState##s_[static_cast<vdl::uint>(ShaderType)];\
\
if (const vdl::uint RequiredSize = _StartSlot + _##ShaderState##Num;\
  Current##ShaderState##s.size() < RequiredSize)\
{\
  Current##ShaderState##s.resize(RequiredSize);\
  StateChangeFlags_.Set(RendererCommandType);\
}\
\
if (!StateChangeFlags_.Has(RendererCommandType))\
{\
  for (vdl::uint ShaderState##Count = 0; ShaderState##Count < _##ShaderState##Num; ++ShaderState##Count)\
  {\
    auto& Current##ShaderState = Current##ShaderState##s[_StartSlot + ShaderState##Count];\
    const auto& ShaderState = _##ShaderState##s[ShaderState##Count];\
    \
    if (Current##ShaderState != ShaderState)\
    {\
      StateChangeFlags_.Set(RendererCommandType);\
      break;\
    }\
  }\
}\
\
for (vdl::uint ShaderState##Count = 0; ShaderState##Count < _##ShaderState##Num; ++ShaderState##Count)\
{\
  Current##ShaderState##s[_StartSlot + ShaderState##Count] = _##ShaderState##s[ShaderState##Count];\
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushVertexStageShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  SetShaderStates(RendererCommandType::eSetVertexStageShaderResource, ShaderType::eVertexShader, ShaderResource)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushHullStageShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  SetShaderStates(RendererCommandType::eSetHullStageShaderResource, ShaderType::eHullShader, ShaderResource)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushDomainStageShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  SetShaderStates(RendererCommandType::eSetDomainStageShaderResource, ShaderType::eDomainShader, ShaderResource)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushGeometryStageShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  SetShaderStates(RendererCommandType::eSetGeometryStageShaderResource, ShaderType::eGeometryShader, ShaderResource)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushPixelStageShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  SetShaderStates(RendererCommandType::eSetPixelStageShaderResource, ShaderType::ePixelShader, ShaderResource)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushVertexStageSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetShaderStates(RendererCommandType::eSetVertexStageSampler, ShaderType::eVertexShader, Sampler)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushHullStageSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetShaderStates(RendererCommandType::eSetVertexStageSampler, ShaderType::eHullShader, Sampler)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushDomainStageSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetShaderStates(RendererCommandType::eSetDomainStageSampler, ShaderType::eDomainShader, Sampler)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushGeometryStageSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetShaderStates(RendererCommandType::eSetGeometryStageSampler, ShaderType::eGeometryShader, Sampler)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushPixelStageSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetShaderStates(RendererCommandType::eSetPixelStageSampler, ShaderType::ePixelShader, Sampler)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushVertexStageConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetShaderStates(RendererCommandType::eSetVertexStageConstantBuffer, ShaderType::eVertexShader, ConstantBuffer)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushHullStageConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetShaderStates(RendererCommandType::eSetHullStageConstantBuffer, ShaderType::eHullShader, ConstantBuffer)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushDomainStageConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetShaderStates(RendererCommandType::eSetDomainStageConstantBuffer, ShaderType::eDomainShader, ConstantBuffer)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushGeometryStageConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetShaderStates(RendererCommandType::eSetGeometryStageConstantBuffer, ShaderType::eGeometryShader, ConstantBuffer)
}

template<class DisplayObject, class InstanceData>
inline void RendererCommandList<DisplayObject, InstanceData>::PushPixelStageConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetShaderStates(RendererCommandType::eSetPixelStageConstantBuffer, ShaderType::ePixelShader, ConstantBuffer)
}

#undef SetShaderStates
