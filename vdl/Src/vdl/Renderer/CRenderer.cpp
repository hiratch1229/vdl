#include "CRenderer.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/ShaderManager/IShaderManager.hpp>
#include <vdl/ModelManager/IModelManager.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vdl/Topology.hpp>
#include <vdl/Macro.hpp>

#include <assert.h>

void CRenderer::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
  pDeviceContext_ = Engine::Get<IDeviceContext>();

  pCameraData_ = std::make_unique<CameraData>();

  pCameraData_->Camera = pCameraData_->Camera = vdl::Camera(vdl::float3(0.0f, 0.0f, -10.0f));
  for (auto& BoneTransform : pCameraData_->ConstantBuffer.GetData().BoneTransforms)
  {
    BoneTransform = vdl::Matrix::Identity();
  }
  pCameraData_->isChange = pCameraData_->isChange = true;

  //  バッファの作成
  {
    IBuffer* pTextureVertexBuffer;
    pDevice_->CreateVertexBuffer(&pTextureVertexBuffer, kRectangle, sizeof(vdl::Vertex2D), static_cast<vdl::uint>(sizeof(vdl::Vertex2D) * vdl::Macro::ArraySize(kRectangle)));
    pTextureVertexBuffer_.reset(pTextureVertexBuffer);

    IBuffer* pTextureInstanceBuffer;
    pDevice_->CreateInstanceBuffer(&pTextureInstanceBuffer, sizeof(Instance2D), sizeof(Instance2D) * Constants::kMaxTextureBatchNum);
    pTextureInstanceBuffer_.reset(pTextureInstanceBuffer);

    IBuffer* pMeshInstanceBuffer;
    pDevice_->CreateInstanceBuffer(&pMeshInstanceBuffer, sizeof(Instance3D), sizeof(Instance3D) * Constants::kMaxMeshBatchNum);
    pMeshInstanceBuffer_.reset(pMeshInstanceBuffer);
  }

  //  描画コマンドリストの初期化
  {
    EmptyRendererCommandList_.Initialize(vdl::TopologyType::ePointList, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D, vdl::Sampler::kDefault3D,
      vdl::VertexShader(), vdl::PixelShader());
    TextureRendererCommandList_.Initialize(vdl::TopologyType::eDefaultTexture, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault2D, vdl::RasterizerState::kDefault2D, vdl::Sampler::kDefault2D,
      vdl::VertexShader(Constants::kDefaultTextureVertexShaderCode, Constants::kDefaultTextureVertexShaderSize, vdl::InputLayoutType::eTexture),
      vdl::PixelShader(Constants::kDefaultTexturePixelShaderCode, Constants::kDefaultTexturePixelShaderSize));
    MeshRendererCommandList_.Initialize(vdl::TopologyType::eDefaultMesh, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D, vdl::Sampler::kDefault3D,
      vdl::VertexShader(Constants::kDefaultMeshVertexShaderCode, Constants::kDefaultMeshVertexShaderSize, vdl::InputLayoutType::eMesh),
      vdl::PixelShader(Constants::kDefaultMeshPixelShaderCode, Constants::kDefaultMeshPixelShaderSize));
  }
}

vdl::Matrix CRenderer::GetView()const
{
  const vdl::Camera& Camera = pCameraData_->Camera;

  return DirectX::XMMatrixLookAtLH({ Camera.Position.x, Camera.Position.y, Camera.Position.z, 1.0f },
    { Camera.Target.x, Camera.Target.y, Camera.Target.z, 1.0f }, { Camera.Up.x, Camera.Up.y, Camera.Up.z, 0.0f });
}

vdl::Matrix CRenderer::GetProjection()const
{
  const vdl::Camera& Camera = pCameraData_->Camera;
  const vdl::Viewport& Viewport = MeshRendererCommandList_.GetCurrentViewport();

  if (Camera.isPerspective)
  {
    const float Aspect = Viewport.Size.x / Viewport.Size.y;
    return DirectX::XMMatrixPerspectiveFovLH(vdl::Math::ToRadian(Camera.Fov), Aspect, Camera.Near, Camera.Far);
  }
  else
  {
    return DirectX::XMMatrixOrthographicLH(Viewport.Size.x, Viewport.Size.y, Camera.Near, Camera.Far);
  }
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
  case vdl::InputLayoutType::eMesh:
    MeshRendererCommandList_.PushTopology(_Topology);
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
  case vdl::InputLayoutType::eMesh:
    MeshRendererCommandList_.PushScissor(_Scissor);
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
  case vdl::InputLayoutType::eMesh:
    MeshRendererCommandList_.PushViewport(_Viewport);
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
  case vdl::InputLayoutType::eMesh:
    MeshRendererCommandList_.PushBlendState(_BlendState);
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
  case vdl::InputLayoutType::eMesh:
    MeshRendererCommandList_.PushDepthStencilState(_DepthStencilState);
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
  case vdl::InputLayoutType::eMesh:
    MeshRendererCommandList_.PushRasterizerState(_RasterizerState);
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
  case vdl::InputLayoutType::eMesh:
    MeshRendererCommandList_.PushVertexShader(_VertexShader);
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
  case vdl::InputLayoutType::eMesh:
    MeshRendererCommandList_.PushHullShader(_HullShader);
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
  case vdl::InputLayoutType::eMesh:
    MeshRendererCommandList_.PushDomainShader(_DomainShader);
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
  case vdl::InputLayoutType::eMesh:
    MeshRendererCommandList_.PushGeometryShader(_GeometryShader);
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
  case vdl::InputLayoutType::eMesh:
    MeshRendererCommandList_.PushPixelShader(_PixelShader);
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
  case vdl::InputLayoutType::eMesh:
    SetShaderResources(&MeshRendererCommandList_);
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
  case vdl::InputLayoutType::eMesh:
    SetSamplers(&MeshRendererCommandList_);
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
  case vdl::InputLayoutType::eMesh:
    SetConstantBuffers(&MeshRendererCommandList_);
    break;
  default: assert(false);
  }
}

void CRenderer::Draw(vdl::uint _VertexCount)
{
  EmptyRendererCommandList_.PushDrawData(Empty(), std::move(_VertexCount));
}

void CRenderer::Draw(const vdl::Texture& _Texture, const vdl::float2& _DestLeftTop, const vdl::float2& _DestSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::ColorF& _Color)
{
  //  表示されないものは除く
  if (_Color.Alpha <= 0.0f || _DestSize.x * _DestSize.y == 0.0f)
  {
    return;
  }

  Instance2D Instance;
  {
    const vdl::Viewport& Viewport = TextureRendererCommandList_.GetCurrentViewport();

    Instance.NDCTransform = vdl::Matrix(DirectX::XMMatrixAffineTransformation2D(DirectX::XMVectorSet(_DestSize.x, _DestSize.y, 0.0f, 0.0f), DirectX::XMVectorZero(),
      _Angle, DirectX::XMVectorSet(_DestLeftTop.x + _DestSize.x * 0.5f, _DestLeftTop.y + _DestSize.y * 0.5f, 0.0f, 0.0f))
      * vdl::Matrix::Scale({ 2.0f / Viewport.Size.x, -2.0f / Viewport.Size.y, 1.0f }) * vdl::Matrix::Translate({ -1.0f, 1.0f, 0.0f })).Transpose();

    const vdl::uint2 TextureSize = _Texture.GetSize();

    Instance.TexcoordScale = _SrcSize / TextureSize;
    Instance.TexcoordTranslate = _SrcLeftPos / TextureSize;

    Instance.Color = _Color;
  }
  TextureRendererCommandList_.PushDrawData(_Texture, std::move(Instance));
}

void CRenderer::Draw(const vdl::Mesh& _Mesh, const vdl::Matrix& _World, const vdl::MotionBlendDatas& _MotionBlendDatas, const vdl::ColorF& _Color)
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
    MeshRendererCommandList_.PushVertexStageConstantBuffers(0, 1, &pCameraData_->ConstantBuffer.GetDetail());
  }

  Instance3D Instance;
  {
    Instance.World = _World * _Mesh.GetGlobalTransform();
    Instance.Color = _Color;
  }
  MeshRendererCommandList_.PushDrawData(_Mesh, std::move(Instance));
}

void CRenderer::Clear(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)
{
  Flush();

  pDeviceContext_->ClearRenderTexture(_RenderTexture, _ClearColor);
}

void CRenderer::Clear(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)
{
  Flush();

  pDeviceContext_->ClearDepthStencilTexture(_DepthStencilTexture, _ClearDepth, _ClearStencil);
}

void CRenderer::Clear(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::ColorF& _ClearColor)
{
  Flush();

  pDeviceContext_->ClearUnorderedAccessTexture(_UnorderedAccessTexture, _ClearColor);
}

void CRenderer::Flush()
{
  const bool HasEmptyDrawCommand = EmptyRendererCommandList_.HasDrawCommand();
  const bool HasStaticMeshDrawCommand = MeshRendererCommandList_.HasDrawCommand();
  const bool HasTextureDrawCommand = TextureRendererCommandList_.HasDrawCommand();

  if (HasEmptyDrawCommand || HasStaticMeshDrawCommand || HasTextureDrawCommand)
  {
    //  それぞれのドローコールをソート(マルチスレッド)
    {
      if (HasEmptyDrawCommand)
      {
        EmptyRendererCommandList_.Adjust();
      }
      if (HasStaticMeshDrawCommand)
      {
        MeshRendererCommandList_.Adjust();
      }
      if (HasTextureDrawCommand)
      {
        TextureRendererCommandList_.Adjust();
      }
    }

    pDeviceContext_->SetRenderTextures(OutputManager_.RenderTextures, OutputManager_.DepthStencilTexture);

    if (HasStaticMeshDrawCommand)
    {
      pDeviceContext_->SetInputLayout(vdl::InputLayoutType::eMesh);
      MeshRendererCommandList_.Flush(pDeviceContext_, pMeshInstanceBuffer_.get());
      MeshRendererCommandList_.Reset();
    }
    if (HasEmptyDrawCommand)
    {
      pDeviceContext_->SetInputLayout(vdl::InputLayoutType::eNone);
      EmptyRendererCommandList_.Flush(pDeviceContext_, nullptr);
      EmptyRendererCommandList_.Reset();
    }
    if (HasTextureDrawCommand)
    {
      pDeviceContext_->SetInputLayout(vdl::InputLayoutType::eTexture);
      pDeviceContext_->SetVertexBuffer(pTextureVertexBuffer_.get());
      TextureRendererCommandList_.Flush(pDeviceContext_, pTextureInstanceBuffer_.get());
      TextureRendererCommandList_.Reset();

      vdl::ShaderResource EmptyTexture = vdl::Texture();
      pDeviceContext_->PSSetShaderResources(0, 1, &EmptyTexture);
    }

    pDeviceContext_->Flush();
  }
}
