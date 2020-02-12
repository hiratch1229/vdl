#include "CRenderer.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>
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

  OutputManager_.RenderTextures[0] = pDeviceContext_->GetRenderTexture();
  OutputManager_.DepthStencilTexture = pDeviceContext_->GetDepthStencilTexture();

  pCameraData_ = std::make_unique<CameraData>();
  pCameraData_->Camera = pCameraData_->Camera = vdl::Camera(vdl::float3(0.0f, 0.0f, -10.0f));
  pCameraData_->isChange = pCameraData_->isChange = true;

  //  描画コマンドリストの初期化
  {
    EmptyGraphicsCommandList_.Initialize(vdl::InputLayoutType::eNone, vdl::TopologyType::eDefaultNone, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D, vdl::Sampler::kDefault3D);

    TextureGraphicsCommandList_.Initialize(vdl::InputLayoutType::eTexture, vdl::TopologyType::eDefaultTexture, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault2D, vdl::RasterizerState::kDefault2D, vdl::Sampler::kDefault2D,
      vdl::VertexShader(Constants::kDefaultTextureShaderCode, Constants::kDefaultTextureShaderSize, vdl::InputLayoutType::eTexture, Constants::kDefaultShaderEntryPointVS),
      vdl::PixelShader(Constants::kDefaultTextureShaderCode, Constants::kDefaultTextureShaderSize, Constants::kDefaultShaderEntryPointPS),
      VertexBuffer(kRectangleVertices, static_cast<vdl::uint>(vdl::Macro::ArraySize(kRectangleVertices)) * sizeof(vdl::Vertex2D)), InstanceBuffer(sizeof(vdl::Instance2D) * Constants::kMaxTextureBatchNum));

    StaticMeshGraphicsCommandList_.Initialize(vdl::InputLayoutType::eStaticMesh, vdl::TopologyType::eDefaultMesh, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D, vdl::Sampler::kDefault3D,
      vdl::VertexShader(Constants::kDefaultStaticMeshShaderCode, Constants::kDefaultStaticMeshShaderSize, vdl::InputLayoutType::eStaticMesh, Constants::kDefaultShaderEntryPointVS),
      vdl::PixelShader(Constants::kDefaultStaticMeshShaderCode, Constants::kDefaultStaticMeshShaderSize, Constants::kDefaultShaderEntryPointPS),
      InstanceBuffer(sizeof(vdl::InstanceStaticMesh) * Constants::kMaxStaticMeshBatchNum));

    //SkinnedMeshGraphicsCommandList_.Initialize(vdl::InputLayoutType::eStaticMesh, vdl::TopologyType::eDefaultMesh, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D, vdl::Sampler::kDefault3D,
    //  vdl::VertexShader(Constants::kDefaultSkinnedMeshShaderCode, Constants::kDefaultSkinnedMeshShaderSize, vdl::InputLayoutType::eSkinnedMesh, Constants::kDefaultShaderEntryPointVS),
    //  vdl::PixelShader(Constants::kDefaultSkinnedMeshShaderCode, Constants::kDefaultSkinnedMeshShaderSize, Constants::kDefaultShaderEntryPointPS),
    //  InstanceBuffer(sizeof(vdl::InstanceStaticMesh) * Constants::kMaxSkinnedMeshBatchNum));
  }
}

vdl::Matrix CRenderer::GetView()const
{
  return pCameraData_->Camera.View();
}

vdl::Matrix CRenderer::GetProjection()const
{
  return pCameraData_->Camera.Projection(StaticMeshGraphicsCommandList_.GetCurrentViewport().Size);
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
    EmptyGraphicsCommandList_.SetTopology(_Topology);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureGraphicsCommandList_.SetTopology(_Topology);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshGraphicsCommandList_.SetTopology(_Topology);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SkinnedMeshGraphicsCommandList_.SetTopology(_Topology);
    break;
  default: assert(false);
  }
}

void CRenderer::SetScissor(const vdl::Scissor& _Scissor, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyGraphicsCommandList_.SetScissor(_Scissor);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureGraphicsCommandList_.SetScissor(_Scissor);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshGraphicsCommandList_.SetScissor(_Scissor);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SkinnedMeshGraphicsCommandList_.SetScissor(_Scissor);
    break;
  default: assert(false);
  }
}

void CRenderer::SetViewport(const vdl::Viewport& _Viewport, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyGraphicsCommandList_.SetViewport(_Viewport);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureGraphicsCommandList_.SetViewport(_Viewport);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshGraphicsCommandList_.SetViewport(_Viewport);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SkinnedMeshGraphicsCommandList_.SetViewport(_Viewport);
    break;
  default: assert(false);
  }
}

void CRenderer::SetBlendState(const vdl::BlendState& _BlendState, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyGraphicsCommandList_.SetBlendState(_BlendState);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureGraphicsCommandList_.SetBlendState(_BlendState);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshGraphicsCommandList_.SetBlendState(_BlendState);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SkinnedMeshGraphicsCommandList_.SetBlendState(_BlendState);
    break;
  default: assert(false);
  }
}

void CRenderer::SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyGraphicsCommandList_.SetDepthStencilState(_DepthStencilState);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureGraphicsCommandList_.SetDepthStencilState(_DepthStencilState);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshGraphicsCommandList_.SetDepthStencilState(_DepthStencilState);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SkinnedMeshGraphicsCommandList_.SetDepthStencilState(_DepthStencilState);
    break;
  default: assert(false);
  }
}

void CRenderer::SetRasterizerState(const vdl::RasterizerState& _RasterizerState, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyGraphicsCommandList_.SetRasterizerState(_RasterizerState);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureGraphicsCommandList_.SetRasterizerState(_RasterizerState);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshGraphicsCommandList_.SetRasterizerState(_RasterizerState);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SkinnedMeshGraphicsCommandList_.SetRasterizerState(_RasterizerState);
    break;
  default: assert(false);
  }
}

void CRenderer::SetVertexShader(const vdl::VertexShader& _VertexShader, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyGraphicsCommandList_.SetVertexShader(_VertexShader);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureGraphicsCommandList_.SetVertexShader(_VertexShader);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshGraphicsCommandList_.SetVertexShader(_VertexShader);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SkinnedMeshGraphicsCommandList_.SetVertexShader(_VertexShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetHullShader(const vdl::HullShader& _HullShader, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyGraphicsCommandList_.SetHullShader(_HullShader);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureGraphicsCommandList_.SetHullShader(_HullShader);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshGraphicsCommandList_.SetHullShader(_HullShader);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SkinnedMeshGraphicsCommandList_.SetHullShader(_HullShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetDomainShader(const vdl::DomainShader& _DomainShader, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyGraphicsCommandList_.SetDomainShader(_DomainShader);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureGraphicsCommandList_.SetDomainShader(_DomainShader);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshGraphicsCommandList_.SetDomainShader(_DomainShader);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SkinnedMeshGraphicsCommandList_.SetDomainShader(_DomainShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetGeometryShader(const vdl::GeometryShader& _GeometryShader, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyGraphicsCommandList_.SetGeometryShader(_GeometryShader);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureGraphicsCommandList_.SetGeometryShader(_GeometryShader);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshGraphicsCommandList_.SetGeometryShader(_GeometryShader);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SkinnedMeshGraphicsCommandList_.SetGeometryShader(_GeometryShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetPixelShader(const vdl::PixelShader& _PixelShader, vdl::InputLayoutType _Type)
{
  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    EmptyGraphicsCommandList_.SetPixelShader(_PixelShader);
    break;
  case vdl::InputLayoutType::eTexture:
    TextureGraphicsCommandList_.SetPixelShader(_PixelShader);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    StaticMeshGraphicsCommandList_.SetPixelShader(_PixelShader);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SkinnedMeshGraphicsCommandList_.SetPixelShader(_PixelShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[], ShaderType _Stage, vdl::InputLayoutType _Type)
{
  auto SetShaderResources = [&](auto* _pGraphicsCommandList)->void
  {
    switch (_Stage)
    {
    case ShaderType::eVertexShader:
      _pGraphicsCommandList->SetShaderResources<ShaderType::eVertexShader>(_StartSlot, _ShaderResourceNum, _ShaderResources);
      break;
    case ShaderType::eHullShader:
      _pGraphicsCommandList->SetShaderResources<ShaderType::eHullShader>(_StartSlot, _ShaderResourceNum, _ShaderResources);
      break;
    case ShaderType::eDomainShader:
      _pGraphicsCommandList->SetShaderResources<ShaderType::eDomainShader>(_StartSlot, _ShaderResourceNum, _ShaderResources);
      break;
    case ShaderType::eGeometryShader:
      _pGraphicsCommandList->SetShaderResources<ShaderType::eGeometryShader>(_StartSlot, _ShaderResourceNum, _ShaderResources);
      break;
    case ShaderType::ePixelShader:
      _pGraphicsCommandList->SetShaderResources<ShaderType::ePixelShader>(_StartSlot, _ShaderResourceNum, _ShaderResources);
      break;
    default: assert(false);
    }
  };

  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    SetShaderResources(&EmptyGraphicsCommandList_);
    break;
  case vdl::InputLayoutType::eTexture:
    SetShaderResources(&TextureGraphicsCommandList_);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    SetShaderResources(&StaticMeshGraphicsCommandList_);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SetShaderResources(&SkinnedMeshGraphicsCommandList_);
    break;
  default: assert(false);
  }
}

void CRenderer::SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[], ShaderType _Stage, vdl::InputLayoutType _Type)
{
  auto SetSamplers = [&](auto* _pGraphicsCommandList)->void
  {
    switch (_Stage)
    {
    case ShaderType::eVertexShader:
      _pGraphicsCommandList->SetSamplers<ShaderType::eVertexShader>(_StartSlot, _SamplerNum, _Samplers);
      break;
    case ShaderType::eHullShader:
      _pGraphicsCommandList->SetSamplers<ShaderType::eHullShader>(_StartSlot, _SamplerNum, _Samplers);
      break;
    case ShaderType::eDomainShader:
      _pGraphicsCommandList->SetSamplers<ShaderType::eDomainShader>(_StartSlot, _SamplerNum, _Samplers);
      break;
    case ShaderType::eGeometryShader:
      _pGraphicsCommandList->SetSamplers<ShaderType::eGeometryShader>(_StartSlot, _SamplerNum, _Samplers);
      break;
    case ShaderType::ePixelShader:
      _pGraphicsCommandList->SetSamplers<ShaderType::ePixelShader>(_StartSlot, _SamplerNum, _Samplers);
      break;
    default: assert(false);
    }
  };

  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    SetSamplers(&EmptyGraphicsCommandList_);
    break;
  case vdl::InputLayoutType::eTexture:
    SetSamplers(&TextureGraphicsCommandList_);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    SetSamplers(&StaticMeshGraphicsCommandList_);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SetSamplers(&SkinnedMeshGraphicsCommandList_);
    break;
  default: assert(false);
  }
}

void CRenderer::SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[], ShaderType _Stage, vdl::InputLayoutType _Type)
{
  auto SetConstantBuffers = [&](auto* _pGraphicsCommandList)->void
  {
    switch (_Stage)
    {
    case ShaderType::eVertexShader:
      _pGraphicsCommandList->SetConstantBuffers<ShaderType::eVertexShader>(_StartSlot, _ConstantBufferNum, _ConstantBuffers);
      break;
    case ShaderType::eHullShader:
      _pGraphicsCommandList->SetConstantBuffers<ShaderType::eHullShader>(_StartSlot, _ConstantBufferNum, _ConstantBuffers);
      break;
    case ShaderType::eDomainShader:
      _pGraphicsCommandList->SetConstantBuffers<ShaderType::eDomainShader>(_StartSlot, _ConstantBufferNum, _ConstantBuffers);
      break;
    case ShaderType::eGeometryShader:
      _pGraphicsCommandList->SetConstantBuffers<ShaderType::eGeometryShader>(_StartSlot, _ConstantBufferNum, _ConstantBuffers);
      break;
    case ShaderType::ePixelShader:
      _pGraphicsCommandList->SetConstantBuffers<ShaderType::ePixelShader>(_StartSlot, _ConstantBufferNum, _ConstantBuffers);
      break;
    default: assert(false);
    }
  };

  switch (_Type)
  {
  case vdl::InputLayoutType::eNone:
    SetConstantBuffers(&EmptyGraphicsCommandList_);
    break;
  case vdl::InputLayoutType::eTexture:
    SetConstantBuffers(&TextureGraphicsCommandList_);
    break;
  case vdl::InputLayoutType::eStaticMesh:
    SetConstantBuffers(&StaticMeshGraphicsCommandList_);
    break;
  case vdl::InputLayoutType::eSkinnedMesh:
    //SetConstantBuffers(&SkinnedMeshGraphicsCommandList_);
    break;
  default: assert(false);
  }
}

void CRenderer::Draw(vdl::uint _VertexCount)
{
  EmptyGraphicsCommandList_.SetDrawData({ _VertexCount, 1, 0, 1 });
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
    const vdl::Viewport& CurrentViewport = TextureGraphicsCommandList_.GetCurrentViewport();

    Instance.NDCTransform = (vdl::Matrix::Scale({ _DestSize, 0.0f }) * vdl::Matrix::RotateZ(_Angle) * vdl::Matrix::Translate({ _DestLeftTop + _DestSize * 0.5f, 0.0f })
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
  TextureGraphicsCommandList_.SetDrawData(_Texture, std::move(Instance));
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
    pCameraData_->isChange = false;
    pCameraData_->ConstantBuffer.GetData().ViewProjection = GetView() * GetProjection();
    StaticMeshGraphicsCommandList_.SetConstantBuffers<ShaderType::eVertexShader>(0, 1, &pCameraData_->ConstantBuffer.GetDetail());
  }

  vdl::InstanceStaticMesh Instance;
  {
    Instance.World = _StaticMesh.GetGlobalTransform() * _World;
    Instance.Color = _Color;
  }
  StaticMeshGraphicsCommandList_.SetDrawData(_StaticMesh, std::move(Instance));
}

void CRenderer::Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const vdl::MotionBlendDatas& _MotionBlendDatas, const vdl::Color4F& _Color)
{
  ////  表示されないものは除く
  //if (_Color.Alpha <= 0.0f)
  //{
  //  return;
  //}
  //
  //if (pCameraData_->isChange)
  //{
  //  //pCameraData_->isChange = false;
  //  pCameraData_->ConstantBuffer.GetData().ViewProjection = GetView() * GetProjection();
  //  SkinnedMeshGraphicsCommandList_.SetConstantBuffers<ShaderType::eVertexShader>(0, 1, &pCameraData_->ConstantBuffer.GetDetail());
  //}
  //
  //vdl::InstanceSkinnedMesh Instance;
  //{
  //  Instance.World = _World * _SkinnedMesh.GetGlobalTransform();
  //  Instance.Color = _Color;
  //}
  //SkinnedMeshGraphicsCommandList_.SetDrawData(_SkinnedMesh, std::move(Instance));
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
  const bool HasEmptyDrawCommand = EmptyGraphicsCommandList_.HasDrawCommand();
  const bool HasTextureDrawCommand = TextureGraphicsCommandList_.HasDrawCommand();
  const bool HasStaticMeshDrawCommand = StaticMeshGraphicsCommandList_.HasDrawCommand();
  //const bool HasSkinnedMeshDrawCommand = SkinnedMeshGraphicsCommandList_.HasDrawCommand();

  pDeviceContext_->SetRenderTextures(OutputManager_.RenderTextures, OutputManager_.DepthStencilTexture);

  if (HasEmptyDrawCommand || HasTextureDrawCommand || HasStaticMeshDrawCommand /*|| HasSkinnedMeshDrawCommand*/)
  {
    ////  それぞれのドローコールをソート(マルチスレッド)
    //{
    //  if (HasEmptyDrawCommand)
    //  {
    //    EmptyGraphicsCommandList_.Adjust();
    //  }
    //  if (HasTextureDrawCommand)
    //  {
    //    TextureGraphicsCommandList_.Adjust();
    //  }
    //  if (HasStaticMeshDrawCommand)
    //  {
    //    StaticMeshGraphicsCommandList_.Adjust();
    //  }
    //  //if (HasSkinnedMeshDrawCommand)
    //  //{
    //  //  SkinnedMeshGraphicsCommandList_.Adjust();
    //  //}
    //}

    if (HasStaticMeshDrawCommand)
    {
      pDeviceContext_->Execute(StaticMeshGraphicsCommandList_);
      StaticMeshGraphicsCommandList_.Reset();
    }
    //if (HasSkinnedMeshDrawCommand)
    //{
    //  SkinnedMeshGraphicsCommandList_.Flush(pDeviceContext_, SkinnedMeshInstanceBuffer_);
    //  SkinnedMeshGraphicsCommandList_.Reset();
    //}
    if (HasEmptyDrawCommand)
    {
      pDeviceContext_->Execute(EmptyGraphicsCommandList_);
      EmptyGraphicsCommandList_.Reset();
    }
    if (HasTextureDrawCommand)
    {
      pDeviceContext_->Execute(TextureGraphicsCommandList_);
      TextureGraphicsCommandList_.Reset();
    }

    pDeviceContext_->Flush();
  }
}
