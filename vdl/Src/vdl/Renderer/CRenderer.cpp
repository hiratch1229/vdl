#include "CRenderer.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>
#include <vdl/SwapChain/ISwapChain.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/ShaderManager/IShaderManager.hpp>
#include <vdl/ModelManager/IModelManager.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vdl/Platform.hpp>
#include <vdl/Topology.hpp>
#include <vdl/Macro.hpp>
#include <vdl/DetectMemoryLeak.hpp>

#include <assert.h>

void CRenderer::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
  pDeviceContext_ = Engine::Get<IDeviceContext>();

  ISwapChain* pSwapchain = Engine::Get<ISwapChain>();

  OutputManager_.RenderTextures[0] = pSwapchain->GetRenderTexture();
  OutputManager_.DepthStencilTexture = pSwapchain->GetDepthStencilTexture();

  pCameraData_ = std::make_unique<CameraData>();
  pCameraData_->Camera = pCameraData_->Camera = vdl::Camera(vdl::float3(0.0f, 0.0f, -10.0f));
  pCameraData_->isChange = pCameraData_->isChange = true;

  //  バッファの作成
  {
    TextureVertexBuffer_ = VertexBuffer(kRectangleVertices, static_cast<vdl::uint>(vdl::Macro::ArraySize(kRectangleVertices)) * sizeof(vdl::Vertex2D));

    TextureInstanceBuffer_ = InstanceBuffer(sizeof(vdl::Instance2D) * Constants::kMaxTextureBatchNum);
    StaticMeshInstanceBuffer_ = InstanceBuffer(sizeof(vdl::InstanceStaticMesh) * Constants::kMaxStaticMeshBatchNum);
    SkinnedMeshInstanceBuffer_ = InstanceBuffer(sizeof(vdl::InstanceSkinnedMesh) * Constants::kMaxSkinnedMeshBatchNum);
  }

  //  描画コマンドリストの初期化
  {
    EmptyRendererCommandList_.Initialize(vdl::TopologyType::eDefaultNone, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D, vdl::Sampler::kDefault3D,
      vdl::VertexShader(), vdl::PixelShader());
    TextureRendererCommandList_.Initialize(vdl::TopologyType::eDefaultTexture, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault2D, vdl::RasterizerState::kDefault2D, vdl::Sampler::kDefault2D,
      vdl::VertexShader(Constants::kDefaultTextureShaderCode, Constants::kDefaultTextureShaderSize, vdl::InputLayoutType::eTexture, Constants::kDefaultShaderEntryPointVS),
      vdl::PixelShader(Constants::kDefaultTextureShaderCode, Constants::kDefaultTextureShaderSize, Constants::kDefaultShaderEntryPointPS));
    StaticMeshRendererCommandList_.Initialize(vdl::TopologyType::eDefaultMesh, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D, vdl::Sampler::kDefault3D,
      vdl::VertexShader(Constants::kDefaultStaticMeshShaderCode, Constants::kDefaultStaticMeshShaderSize, vdl::InputLayoutType::eStaticMesh, Constants::kDefaultShaderEntryPointVS),
      vdl::PixelShader(Constants::kDefaultStaticMeshShaderCode, Constants::kDefaultStaticMeshShaderSize, Constants::kDefaultShaderEntryPointPS));
    SkinnedMeshRendererCommandList_.Initialize(vdl::TopologyType::eDefaultMesh, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D, vdl::Sampler::kDefault3D,
      vdl::VertexShader(Constants::kDefaultSkinnedMeshShaderCode, Constants::kDefaultSkinnedMeshShaderSize, vdl::InputLayoutType::eSkinnedMesh, Constants::kDefaultShaderEntryPointVS),
      vdl::PixelShader(Constants::kDefaultSkinnedMeshShaderCode, Constants::kDefaultSkinnedMeshShaderSize, Constants::kDefaultShaderEntryPointPS));
  }
}

vdl::Matrix CRenderer::GetView()const
{
  return pCameraData_->Camera.View();
}

vdl::Matrix CRenderer::GetProjection()const
{
  return pCameraData_->Camera.Projection(StaticMeshRendererCommandList_.GetCurrentViewport().Size);
}

void CRenderer::SetCamera(const vdl::Camera& _Camera)
{
  pCameraData_->Camera = _Camera;
  pCameraData_->isChange = true;
}

void CRenderer::SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStencilTexture)
{
  vdl::OutputManager OutputManager{ _RenderTextures, _DepthStencilTexture };

  if (OutputManager_ != OutputManager)
  {
    Flush();
    OutputManager_ = std::move(OutputManager);
  }
}

void CRenderer::SetTopology(vdl::TopologyType _Topology, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyRendererCommandList_.PushTopology(_Topology);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureRendererCommandList_.PushTopology(_Topology);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshRendererCommandList_.PushTopology(_Topology);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushTopology(_Topology);
    break;
  default: assert(false);
  }
}

void CRenderer::SetScissor(const vdl::Scissor& _Scissor, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyRendererCommandList_.PushScissor(_Scissor);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureRendererCommandList_.PushScissor(_Scissor);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshRendererCommandList_.PushScissor(_Scissor);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushScissor(_Scissor);
    break;
  default: assert(false);
  }
}

void CRenderer::SetViewport(const vdl::Viewport& _Viewport, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyRendererCommandList_.PushViewport(_Viewport);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureRendererCommandList_.PushViewport(_Viewport);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshRendererCommandList_.PushViewport(_Viewport);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushViewport(_Viewport);
    break;
  default: assert(false);
  }
}

void CRenderer::SetBlendState(const vdl::BlendState& _BlendState, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyRendererCommandList_.PushBlendState(_BlendState);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureRendererCommandList_.PushBlendState(_BlendState);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshRendererCommandList_.PushBlendState(_BlendState);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushBlendState(_BlendState);
    break;
  default: assert(false);
  }
}

void CRenderer::SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyRendererCommandList_.PushDepthStencilState(_DepthStencilState);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureRendererCommandList_.PushDepthStencilState(_DepthStencilState);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshRendererCommandList_.PushDepthStencilState(_DepthStencilState);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushDepthStencilState(_DepthStencilState);
    break;
  default: assert(false);
  }
}

void CRenderer::SetRasterizerState(const vdl::RasterizerState& _RasterizerState, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyRendererCommandList_.PushRasterizerState(_RasterizerState);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureRendererCommandList_.PushRasterizerState(_RasterizerState);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshRendererCommandList_.PushRasterizerState(_RasterizerState);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushRasterizerState(_RasterizerState);
    break;
  default: assert(false);
  }
}

void CRenderer::SetVertexShader(const vdl::VertexShader& _VertexShader, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyRendererCommandList_.PushVertexShader(_VertexShader);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureRendererCommandList_.PushVertexShader(_VertexShader);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshRendererCommandList_.PushVertexShader(_VertexShader);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushVertexShader(_VertexShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetHullShader(const vdl::HullShader& _HullShader, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyRendererCommandList_.PushHullShader(_HullShader);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureRendererCommandList_.PushHullShader(_HullShader);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshRendererCommandList_.PushHullShader(_HullShader);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushHullShader(_HullShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetDomainShader(const vdl::DomainShader& _DomainShader, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyRendererCommandList_.PushDomainShader(_DomainShader);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureRendererCommandList_.PushDomainShader(_DomainShader);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshRendererCommandList_.PushDomainShader(_DomainShader);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushDomainShader(_DomainShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetGeometryShader(const vdl::GeometryShader& _GeometryShader, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyRendererCommandList_.PushGeometryShader(_GeometryShader);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureRendererCommandList_.PushGeometryShader(_GeometryShader);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshRendererCommandList_.PushGeometryShader(_GeometryShader);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushGeometryShader(_GeometryShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetPixelShader(const vdl::PixelShader& _PixelShader, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyRendererCommandList_.PushPixelShader(_PixelShader);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureRendererCommandList_.PushPixelShader(_PixelShader);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshRendererCommandList_.PushPixelShader(_PixelShader);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushPixelShader(_PixelShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[], ShaderType _Stage, vdl::InputLayoutType _Type)
{
  auto SetShaderResources = [&](auto* _pRendererCommandList)->void
  {
    switch (_Stage)
    {
    case ShaderType::eVertexShader:
      _pRendererCommandList->PushVertexStageShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources);
      break;
    case ShaderType::eHullShader:
      _pRendererCommandList->PushHullStageShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources);
      break;
    case ShaderType::eDomainShader:
      _pRendererCommandList->PushDomainStageShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources);
      break;
    case ShaderType::eGeometryShader:
      _pRendererCommandList->PushGeometryStageShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources);
      break;
    case ShaderType::ePixelShader:
      _pRendererCommandList->PushPixelStageShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources);
      break;
    default: assert(false);
    }
  };

  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    SetShaderResources(&EmptyRendererCommandList_);
    break;
  case vdl::InputLayoutType::eTexture:
    SetShaderResources(&TextureRendererCommandList_);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    SetShaderResources(&StaticMeshRendererCommandList_);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SetShaderResources(&SkinnedMeshRendererCommandList_);
    break;
  default: assert(false);
  }
}

void CRenderer::SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[], ShaderType _Stage, vdl::InputLayoutType _Type)
{
  auto SetSamplers = [&](auto* _pRendererCommandList)->void
  {
    switch (_Stage)
    {
    case ShaderType::eVertexShader:
      _pRendererCommandList->PushVertexStageSamplers(_StartSlot, _SamplerNum, _Samplers);
      break;
    case ShaderType::eHullShader:
      _pRendererCommandList->PushHullStageSamplers(_StartSlot, _SamplerNum, _Samplers);
      break;
    case ShaderType::eDomainShader:
      _pRendererCommandList->PushDomainStageSamplers(_StartSlot, _SamplerNum, _Samplers);
      break;
    case ShaderType::eGeometryShader:
      _pRendererCommandList->PushGeometryStageSamplers(_StartSlot, _SamplerNum, _Samplers);
      break;
    case ShaderType::ePixelShader:
      _pRendererCommandList->PushPixelStageSamplers(_StartSlot, _SamplerNum, _Samplers);
      break;
    default: assert(false);
    }
  };

  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    SetSamplers(&EmptyRendererCommandList_);
    break;
  case vdl::InputLayoutType::eTexture:
    SetSamplers(&TextureRendererCommandList_);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    SetSamplers(&StaticMeshRendererCommandList_);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SetSamplers(&SkinnedMeshRendererCommandList_);
    break;
  default: assert(false);
  }
}

void CRenderer::SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[], ShaderType _Stage, vdl::InputLayoutType _Type)
{
  auto SetConstantBuffers = [&](auto* _pRendererCommandList)->void
  {
    switch (_Stage)
    {
    case ShaderType::eVertexShader:
      _pRendererCommandList->PushVertexStageConstantBuffers(_StartSlot, _ConstantBufferNum, _ConstantBuffers);
      break;
    case ShaderType::eHullShader:
      _pRendererCommandList->PushHullStageConstantBuffers(_StartSlot, _ConstantBufferNum, _ConstantBuffers);
      break;
    case ShaderType::eDomainShader:
      _pRendererCommandList->PushDomainStageConstantBuffers(_StartSlot, _ConstantBufferNum, _ConstantBuffers);
      break;
    case ShaderType::eGeometryShader:
      _pRendererCommandList->PushGeometryStageConstantBuffers(_StartSlot, _ConstantBufferNum, _ConstantBuffers);
      break;
    case ShaderType::ePixelShader:
      _pRendererCommandList->PushPixelStageConstantBuffers(_StartSlot, _ConstantBufferNum, _ConstantBuffers);
      break;
    default: assert(false);
    }
  };

  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    SetConstantBuffers(&EmptyRendererCommandList_);
    break;
  case vdl::InputLayoutType::eTexture:
    SetConstantBuffers(&TextureRendererCommandList_);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    SetConstantBuffers(&StaticMeshRendererCommandList_);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    SetConstantBuffers(&SkinnedMeshRendererCommandList_);
    break;
  default: assert(false);
  }
}

void CRenderer::Draw(vdl::uint _VertexCount)
{
  EmptyRendererCommandList_.PushDrawData(Empty(), std::move(_VertexCount));
}

void CRenderer::Draw(const vdl::Texture& _Texture, const vdl::float2& _DestLeftTop, const vdl::float2& _DestSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::Color4F& _Color)
{
  //  表示されないものは除く
  if (_Color.Alpha <= 0.0f || _DestSize.x * _DestSize.y == 0.0f)
  {
    return;
  }

  vdl::Instance2D Instance;
  {
    const vdl::Viewport& CurrentViewport = TextureRendererCommandList_.GetCurrentViewport();

    Instance.NDCTransform = vdl::Matrix(DirectX::XMMatrixAffineTransformation2D(DirectX::XMVectorSet(_DestSize.x, _DestSize.y, 0.0f, 0.0f), DirectX::XMVectorZero(),
      _Angle, DirectX::XMVectorSet(_DestLeftTop.x + _DestSize.x * 0.5f, _DestLeftTop.y + _DestSize.y * 0.5f, 0.0f, 0.0f))
      * vdl::Matrix::Scale({ 2.0f / CurrentViewport.Size.x, -2.0f / CurrentViewport.Size.y, 1.0f }) * vdl::Matrix::Translate({ -1.0f, 1.0f, 0.0f }))
#if !defined VDL_EXECUTE_VULKAN
      .Transpose()
#endif
      ;

    const vdl::uint2 TextureSize = _Texture.GetSize();

    Instance.TexcoordScale = _SrcSize / TextureSize;
    Instance.TexcoordTranslate = _SrcLeftPos / TextureSize;

    Instance.Color = _Color;
  }
  TextureRendererCommandList_.PushDrawData(_Texture, std::move(Instance));
}

void CRenderer::Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::Color4F& _Color)
{
  //  表示されないものは除く
  if (_Color.Alpha <= 0.0f)
  {
    return;
  }

  if (pCameraData_->isChange)
  {
    //pCameraData_->isChange = false;
    pCameraData_->ConstantBuffer.GetData().ViewProjection = GetView() * GetProjection();
    StaticMeshRendererCommandList_.PushVertexStageConstantBuffers(0, 1, &pCameraData_->ConstantBuffer.GetDetail());
  }

  vdl::InstanceStaticMesh Instance;
  {
    Instance.World = _StaticMesh.GetGlobalTransform() * _World;
    Instance.Color = _Color;
  }
  StaticMeshRendererCommandList_.PushDrawData(_StaticMesh, std::move(Instance));
}

void CRenderer::Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const vdl::MotionBlendDatas& _MotionBlendDatas, const vdl::Color4F& _Color)
{
  //  表示されないものは除く
  if (_Color.Alpha <= 0.0f)
  {
    return;
  }

  if (pCameraData_->isChange)
  {
    //pCameraData_->isChange = false;
    pCameraData_->ConstantBuffer.GetData().ViewProjection = GetView() * GetProjection();
    SkinnedMeshRendererCommandList_.PushVertexStageConstantBuffers(0, 1, &pCameraData_->ConstantBuffer.GetDetail());
  }

  vdl::InstanceSkinnedMesh Instance;
  {
    Instance.World = _World * _SkinnedMesh.GetGlobalTransform();
    Instance.Color = _Color;
  }
  SkinnedMeshRendererCommandList_.PushDrawData(_SkinnedMesh, std::move(Instance));
}

void CRenderer::Clear(const vdl::RenderTexture& _RenderTexture, const vdl::Color4F& _ClearColor)
{
  Flush();

  pDeviceContext_->ClearRenderTexture(_RenderTexture, _ClearColor);
}

void CRenderer::Clear(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)
{
  Flush();

  pDeviceContext_->ClearDepthStencilTexture(_DepthStencilTexture, _ClearDepth, _ClearStencil);
}

void CRenderer::Clear(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::Color4F& _ClearColor)
{
  Flush();

  pDeviceContext_->ClearUnorderedAccessTexture(_UnorderedAccessTexture, _ClearColor);
}

void CRenderer::Flush()
{
  const bool HasEmptyDrawCommand = EmptyRendererCommandList_.HasDrawCommand();
  const bool HasTextureDrawCommand = TextureRendererCommandList_.HasDrawCommand();
  const bool HasStaticMeshDrawCommand = StaticMeshRendererCommandList_.HasDrawCommand();
  const bool HasSkinnedMeshDrawCommand = SkinnedMeshRendererCommandList_.HasDrawCommand();

  pDeviceContext_->SetRenderTextures(OutputManager_.RenderTextures, OutputManager_.DepthStencilTexture);

  if (HasEmptyDrawCommand || HasTextureDrawCommand || HasStaticMeshDrawCommand || HasSkinnedMeshDrawCommand)
  {
    //  それぞれのドローコールをソート(マルチスレッド)
    {
      if (HasEmptyDrawCommand)
      {
        EmptyRendererCommandList_.Adjust();
      }
      if (HasTextureDrawCommand)
      {
        TextureRendererCommandList_.Adjust();
      }
      if (HasStaticMeshDrawCommand)
      {
        StaticMeshRendererCommandList_.Adjust();
      }
      if (HasSkinnedMeshDrawCommand)
      {
        SkinnedMeshRendererCommandList_.Adjust();
      }
    }

    if (HasStaticMeshDrawCommand)
    {
      pDeviceContext_->SetInputLayout(vdl::InputLayoutType::eStaticMesh);
      StaticMeshRendererCommandList_.Flush(pDeviceContext_, StaticMeshInstanceBuffer_);
      StaticMeshRendererCommandList_.Reset();
    }
    if (HasSkinnedMeshDrawCommand)
    {
      pDeviceContext_->SetInputLayout(vdl::InputLayoutType::eSkinnedMesh);
      SkinnedMeshRendererCommandList_.Flush(pDeviceContext_, SkinnedMeshInstanceBuffer_);
      SkinnedMeshRendererCommandList_.Reset();
    }
    if (HasEmptyDrawCommand)
    {
      pDeviceContext_->SetInputLayout(vdl::InputLayoutType::eNone);
      EmptyRendererCommandList_.Flush(pDeviceContext_, NoneInstanceBuffer_);
      EmptyRendererCommandList_.Reset();
    }
    if (HasTextureDrawCommand)
    {
      pDeviceContext_->SetInputLayout(vdl::InputLayoutType::eTexture);
      pDeviceContext_->SetVertexBuffer(TextureVertexBuffer_);
      TextureRendererCommandList_.Flush(pDeviceContext_, TextureInstanceBuffer_);
      TextureRendererCommandList_.Reset();
    }

    pDeviceContext_->Flush();
  }
}
