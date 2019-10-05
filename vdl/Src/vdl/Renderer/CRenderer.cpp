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

  pStaticMeshCameraData_ = std::make_unique<CameraData>();
  pSkinnedMeshCameraData_ = std::make_unique<CameraData>();

  pStaticMeshCameraData_->Camera = pSkinnedMeshCameraData_->Camera = vdl::Camera(vdl::float3(0.0f, 0.0f, -10.0f));
  pStaticMeshCameraData_->isChange = pSkinnedMeshCameraData_->isChange = true;

  //  �o�b�t�@�̍쐬
  {
    IBuffer* pTextureVertexBuffer;
    pDevice_->CreateVertexBuffer(&pTextureVertexBuffer, kRectangle, sizeof(vdl::TextureVertex), static_cast<vdl::uint>(sizeof(vdl::TextureVertex) * vdl::Macro::ArraySize(kRectangle)));
    pTextureVertexBuffer_.reset(pTextureVertexBuffer);

    IBuffer* pTextureInstanceBuffer;
    pDevice_->CreateInstanceBuffer(&pTextureInstanceBuffer, sizeof(TextureInstanceData), sizeof(TextureInstanceData) * Constants::kMaxTextureBatchNum);
    pTextureInstanceBuffer_.reset(pTextureInstanceBuffer);

    IBuffer* pStaticMeshInstanceBuffer;
    pDevice_->CreateInstanceBuffer(&pStaticMeshInstanceBuffer, sizeof(StaticMeshInstanceData), sizeof(StaticMeshInstanceData) * Constants::kMaxStaticMeshBatchNum);
    pStaticMeshInstanceBuffer_.reset(pStaticMeshInstanceBuffer);

    IBuffer* pSkinnedMeshInstanceBuffer;
    pDevice_->CreateInstanceBuffer(&pSkinnedMeshInstanceBuffer, sizeof(SkinnedMeshinstanceData), sizeof(SkinnedMeshinstanceData) * Constants::kMaxSkinnedMeshBatchNum);
    pSkinnedMeshInstanceBuffer_.reset(pSkinnedMeshInstanceBuffer);
  }

  //  �`��R�}���h���X�g�̏�����
  {
    EmptyRendererCommandList_.Initialize(vdl::TopologyType::ePointList, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D, vdl::Sampler::kDefault3D,
      vdl::VertexShader(), vdl::PixelShader());
    TextureRendererCommandList_.Initialize(vdl::TopologyType::eDefaultTexture, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault2D, vdl::RasterizerState::kDefault2D, vdl::Sampler::kDefault2D,
      vdl::VertexShader(Constants::kDefaultTextureVertexShaderCode, Constants::kDefaultTextureVertexShaderSize, vdl::InputLayoutType::eTexture),
      vdl::PixelShader(Constants::kDefaultTexturePixelShaderCode, Constants::kDefaultTexturePixelShaderSize));
    StaticMeshRendererCommandList_.Initialize(vdl::TopologyType::eDefaultStaticMesh, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D, vdl::Sampler::kDefault3D,
      vdl::VertexShader(Constants::kDefaultStaticMeshVertexShaderCode, Constants::kDefaultStaticMeshVertexShaderSize, vdl::InputLayoutType::eStaticMesh),
      vdl::PixelShader(Constants::kDefaultStaticMeshPixelShaderCode, Constants::kDefaultStaticMeshPixelShaderSize));
    SkinnedMeshRendererCommandList_.Initialize(vdl::TopologyType::eDefaultSkinnedMesh, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D, vdl::Sampler::kDefault3D,
      vdl::VertexShader(Constants::kDefaultSkinnedMeshVertexShaderCode, Constants::kDefaultSkinnedMeshVertexShaderSize, vdl::InputLayoutType::eSkinnedMesh),
      vdl::PixelShader(Constants::kDefaultSkinnedMeshPixelShaderCode, Constants::kDefaultSkinnedMeshPixelShaderSize));
  }
}

vdl::Matrix CRenderer::GetView(RenderType _Type)const
{
  const vdl::Camera& Camera = (_Type == RenderType::eStaticMesh ? pStaticMeshCameraData_->Camera : pSkinnedMeshCameraData_->Camera);

  return DirectX::XMMatrixLookAtLH({ Camera.Position.x, Camera.Position.y, Camera.Position.z, 1.0f },
    { Camera.Target.x, Camera.Target.y, Camera.Target.z, 1.0f }, { Camera.Up.x, Camera.Up.y, Camera.Up.z, 0.0f });
}

vdl::Matrix CRenderer::GetProjection(RenderType _Type)const
{
  const vdl::Camera& Camera = (_Type == RenderType::eStaticMesh ? pStaticMeshCameraData_->Camera : pSkinnedMeshCameraData_->Camera);
  const vdl::Viewport& Viewport = (_Type == RenderType::eStaticMesh ? StaticMeshRendererCommandList_.GetCurrentViewport() : SkinnedMeshRendererCommandList_.GetCurrentViewport());

  if (Camera.isPerspective)
  {
    const float Aspect = Viewport.Size.x / Viewport.Size.y;
    return DirectX::XMMatrixPerspectiveFovLH(vdl::Math::ToRadian(Camera.Fov), Aspect, Camera.Near, Camera.Far);
  }
  else
  {
    constexpr float kWidth = 16.0f / 2.0f;
    constexpr float kHeight = 9.0f / 2.0f;

    return DirectX::XMMatrixOrthographicLH(kWidth, kHeight, Camera.Near, Camera.Far);
  }
}

void CRenderer::SetCamera(const vdl::Camera& _Camera, RenderType _Type)
{
  CameraData* pCameraData = (_Type == RenderType::eStaticMesh ? pStaticMeshCameraData_.get() : pSkinnedMeshCameraData_.get());
  pCameraData->Camera = _Camera;
  pCameraData->isChange = true;
}

void CRenderer::SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStencilTexture)
{
  OutputManager OutputManager{ _RenderTextures, _DepthStencilTexture };

  if (OutputManager_ != OutputManager)
  {
    Flush();
    OutputManager_ = std::move(OutputManager);
  }
}

void CRenderer::SetTopology(vdl::TopologyType _Topology, RenderType _Type)
{
  switch (_Type)
  {
  case RenderType::eNone:
    EmptyRendererCommandList_.PushTopology(_Topology);
    break;
  case RenderType::eTexture:
    TextureRendererCommandList_.PushTopology(_Topology);
    break;
  case RenderType::eStaticMesh:
    StaticMeshRendererCommandList_.PushTopology(_Topology);
    break;
  case RenderType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushTopology(_Topology);
    break;
  default: assert(false);
  }
}

void CRenderer::SetScissor(const vdl::Scissor& _Scissor, RenderType _Type)
{
  switch (_Type)
  {
  case RenderType::eNone:
    EmptyRendererCommandList_.PushScissor(_Scissor);
    break;
  case RenderType::eTexture:
    TextureRendererCommandList_.PushScissor(_Scissor);
    break;
  case RenderType::eStaticMesh:
    StaticMeshRendererCommandList_.PushScissor(_Scissor);
    break;
  case RenderType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushScissor(_Scissor);
    break;
  default: assert(false);
  }
}

void CRenderer::SetViewport(const vdl::Viewport& _Viewport, RenderType _Type)
{
  switch (_Type)
  {
  case RenderType::eNone:
    EmptyRendererCommandList_.PushViewport(_Viewport);
    break;
  case RenderType::eTexture:
    TextureRendererCommandList_.PushViewport(_Viewport);
    break;
  case RenderType::eStaticMesh:
    StaticMeshRendererCommandList_.PushViewport(_Viewport);
    break;
  case RenderType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushViewport(_Viewport);
    break;
  default: assert(false);
  }
}

void CRenderer::SetBlendState(const vdl::BlendState& _BlendState, RenderType _Type)
{
  switch (_Type)
  {
  case RenderType::eNone:
    EmptyRendererCommandList_.PushBlendState(_BlendState);
    break;
  case RenderType::eTexture:
    TextureRendererCommandList_.PushBlendState(_BlendState);
    break;
  case RenderType::eStaticMesh:
    StaticMeshRendererCommandList_.PushBlendState(_BlendState);
    break;
  case RenderType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushBlendState(_BlendState);
    break;
  default: assert(false);
  }
}

void CRenderer::SetDepthStencilState(const vdl::DepthStencilState& _DepthStencilState, RenderType _Type)
{
  switch (_Type)
  {
  case RenderType::eNone:
    EmptyRendererCommandList_.PushDepthStencilState(_DepthStencilState);
    break;
  case RenderType::eTexture:
    TextureRendererCommandList_.PushDepthStencilState(_DepthStencilState);
    break;
  case RenderType::eStaticMesh:
    StaticMeshRendererCommandList_.PushDepthStencilState(_DepthStencilState);
    break;
  case RenderType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushDepthStencilState(_DepthStencilState);
    break;
  default: assert(false);
  }
}

void CRenderer::SetRasterizerState(const vdl::RasterizerState& _RasterizerState, RenderType _Type)
{
  switch (_Type)
  {
  case RenderType::eNone:
    EmptyRendererCommandList_.PushRasterizerState(_RasterizerState);
    break;
  case RenderType::eTexture:
    TextureRendererCommandList_.PushRasterizerState(_RasterizerState);
    break;
  case RenderType::eStaticMesh:
    StaticMeshRendererCommandList_.PushRasterizerState(_RasterizerState);
    break;
  case RenderType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushRasterizerState(_RasterizerState);
    break;
  default: assert(false);
  }
}

void CRenderer::SetVertexShader(const vdl::VertexShader& _VertexShader, RenderType _Type)
{
  switch (_Type)
  {
  case RenderType::eNone:
    EmptyRendererCommandList_.PushVertexShader(_VertexShader);
    break;
  case RenderType::eTexture:
    TextureRendererCommandList_.PushVertexShader(_VertexShader);
    break;
  case RenderType::eStaticMesh:
    StaticMeshRendererCommandList_.PushVertexShader(_VertexShader);
    break;
  case RenderType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushVertexShader(_VertexShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetHullShader(const vdl::HullShader& _HullShader, RenderType _Type)
{
  switch (_Type)
  {
  case RenderType::eNone:
    EmptyRendererCommandList_.PushHullShader(_HullShader);
    break;
  case RenderType::eTexture:
    TextureRendererCommandList_.PushHullShader(_HullShader);
    break;
  case RenderType::eStaticMesh:
    StaticMeshRendererCommandList_.PushHullShader(_HullShader);
    break;
  case RenderType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushHullShader(_HullShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetDomainShader(const vdl::DomainShader& _DomainShader, RenderType _Type)
{
  switch (_Type)
  {
  case RenderType::eNone:
    EmptyRendererCommandList_.PushDomainShader(_DomainShader);
    break;
  case RenderType::eTexture:
    TextureRendererCommandList_.PushDomainShader(_DomainShader);
    break;
  case RenderType::eStaticMesh:
    StaticMeshRendererCommandList_.PushDomainShader(_DomainShader);
    break;
  case RenderType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushDomainShader(_DomainShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetGeometryShader(const vdl::GeometryShader& _GeometryShader, RenderType _Type)
{
  switch (_Type)
  {
  case RenderType::eNone:
    EmptyRendererCommandList_.PushGeometryShader(_GeometryShader);
    break;
  case RenderType::eTexture:
    TextureRendererCommandList_.PushGeometryShader(_GeometryShader);
    break;
  case RenderType::eStaticMesh:
    StaticMeshRendererCommandList_.PushGeometryShader(_GeometryShader);
    break;
  case RenderType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushGeometryShader(_GeometryShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetPixelShader(const vdl::PixelShader& _PixelShader, RenderType _Type)
{
  switch (_Type)
  {
  case RenderType::eNone:
    EmptyRendererCommandList_.PushPixelShader(_PixelShader);
    break;
  case RenderType::eTexture:
    TextureRendererCommandList_.PushPixelShader(_PixelShader);
    break;
  case RenderType::eStaticMesh:
    StaticMeshRendererCommandList_.PushPixelShader(_PixelShader);
    break;
  case RenderType::eSkinnedMesh:
    SkinnedMeshRendererCommandList_.PushPixelShader(_PixelShader);
    break;
  default: assert(false);
  }
}

void CRenderer::SetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[], ShaderType _Stage, RenderType _Type)
{
  auto SetTexture = [&](auto* _pRendererCommandList)->void
  {
    for (vdl::uint TextureCount = 0; TextureCount < _TextureNum; ++TextureCount)
    {
      switch (_Stage)
      {
      case ShaderType::eVertexShader:
        _pRendererCommandList->PushVertexStageTexture(_Textures[TextureCount], _StartSlot + TextureCount);
        break;
      case ShaderType::eHullShader:
        _pRendererCommandList->PushHullStageTexture(_Textures[TextureCount], _StartSlot + TextureCount);
        break;
      case ShaderType::eDomainShader:
        _pRendererCommandList->PushDomainStageTexture(_Textures[TextureCount], _StartSlot + TextureCount);
        break;
      case ShaderType::eGeometryShader:
        _pRendererCommandList->PushGeometryStageTexture(_Textures[TextureCount], _StartSlot + TextureCount);
        break;
      case ShaderType::ePixelShader:
        _pRendererCommandList->PushPixelStageTexture(_Textures[TextureCount], _StartSlot + TextureCount);
        break;
      default: assert(false);
      }

    }
  };

  switch (_Type)
  {
  case RenderType::eNone:
    SetTexture(&EmptyRendererCommandList_);
    break;
  case RenderType::eTexture:
    SetTexture(&TextureRendererCommandList_);
    break;
  case RenderType::eStaticMesh:
    SetTexture(&StaticMeshRendererCommandList_);
    break;
  case RenderType::eSkinnedMesh:
    SetTexture(&SkinnedMeshRendererCommandList_);
    break;
  default: assert(false);
  }
}

void CRenderer::SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[], ShaderType _Stage, RenderType _Type)
{
  auto SetSampler = [&](auto* _pRendererCommandList)->void
  {
    for (vdl::uint SamplerCount = 0; SamplerCount < _SamplerNum; ++SamplerCount)
    {
      switch (_Stage)
      {
      case ShaderType::eVertexShader:
        _pRendererCommandList->PushVertexStageSampler(_Samplers[SamplerCount], _StartSlot + SamplerCount);
        break;
      case ShaderType::eHullShader:
        _pRendererCommandList->PushHullStageSampler(_Samplers[SamplerCount], _StartSlot + SamplerCount);
        break;
      case ShaderType::eDomainShader:
        _pRendererCommandList->PushDomainStageSampler(_Samplers[SamplerCount], _StartSlot + SamplerCount);
        break;
      case ShaderType::eGeometryShader:
        _pRendererCommandList->PushGeometryStageSampler(_Samplers[SamplerCount], _StartSlot + SamplerCount);
        break;
      case ShaderType::ePixelShader:
        _pRendererCommandList->PushPixelStageSampler(_Samplers[SamplerCount], _StartSlot + SamplerCount);
        break;
      default: assert(false);
      }

    }
  };

  switch (_Type)
  {
  case RenderType::eNone:
    SetSampler(&EmptyRendererCommandList_);
    break;
  case RenderType::eTexture:
    SetSampler(&TextureRendererCommandList_);
    break;
  case RenderType::eStaticMesh:
    SetSampler(&StaticMeshRendererCommandList_);
    break;
  case RenderType::eSkinnedMesh:
    SetSampler(&SkinnedMeshRendererCommandList_);
    break;
  default: assert(false);
  }
}

void CRenderer::SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[], ShaderType _Stage, RenderType _Type)
{
  auto SetConstantBuffer = [&](auto* _pRendererCommandList)->void
  {
    for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < _ConstantBufferNum; ++ConstantBufferCount)
    {
      switch (_Stage)
      {
      case ShaderType::eVertexShader:
        _pRendererCommandList->PushVertexStageConstantBuffer(_ConstantBuffers[ConstantBufferCount], _StartSlot + ConstantBufferCount);
        break;
      case ShaderType::eHullShader:
        _pRendererCommandList->PushHullStageConstantBuffer(_ConstantBuffers[ConstantBufferCount], _StartSlot + ConstantBufferCount);
        break;
      case ShaderType::eDomainShader:
        _pRendererCommandList->PushDomainStageConstantBuffer(_ConstantBuffers[ConstantBufferCount], _StartSlot + ConstantBufferCount);
        break;
      case ShaderType::eGeometryShader:
        _pRendererCommandList->PushGeometryStageConstantBuffer(_ConstantBuffers[ConstantBufferCount], _StartSlot + ConstantBufferCount);
        break;
      case ShaderType::ePixelShader:
        _pRendererCommandList->PushPixelStageConstantBuffer(_ConstantBuffers[ConstantBufferCount], _StartSlot + ConstantBufferCount);
        break;
      default: assert(false);
      }

    }
  };

  switch (_Type)
  {
  case RenderType::eNone:
    SetConstantBuffer(&EmptyRendererCommandList_);
    break;
  case RenderType::eTexture:
    SetConstantBuffer(&TextureRendererCommandList_);
    break;
  case RenderType::eStaticMesh:
    SetConstantBuffer(&StaticMeshRendererCommandList_);
    break;
  case RenderType::eSkinnedMesh:
    SetConstantBuffer(&SkinnedMeshRendererCommandList_);
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
  //  �\������Ȃ����̂͏���
  if (_Color.Alpha <= 0.0f || _DestSize.x * _DestSize.y == 0.0f)
  {
    return;
  }

  TextureInstanceData InstanceData;
  {
    const vdl::Viewport& Viewport = TextureRendererCommandList_.GetCurrentViewport();

    InstanceData.NDCTransform = vdl::Matrix(DirectX::XMMatrixAffineTransformation2D(DirectX::XMVectorSet(_DestSize.x, _DestSize.y, 0.0f, 0.0f), DirectX::XMVectorZero(),
      _Angle, DirectX::XMVectorSet(_DestLeftTop.x + _DestSize.x * 0.5f, _DestLeftTop.y + _DestSize.y * 0.5f, 0.0f, 0.0f))
      * vdl::Matrix::Scale({ 2.0f / Viewport.Size.x, -2.0f / Viewport.Size.y, 1.0f }) * vdl::Matrix::Translate({ -1.0f, 1.0f, 0.0f })).Transpose();

    const vdl::uint2 TextureSize = _Texture.GetSize();

    InstanceData.TexcoordScale = _SrcSize / TextureSize;
    InstanceData.TexcoordTranslate = _SrcLeftPos / TextureSize;

    InstanceData.Color = _Color;
  }
  TextureRendererCommandList_.PushDrawData(_Texture, std::move(InstanceData));
}

void CRenderer::Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::ColorF& _Color)
{
  //  �\������Ȃ����̂͏���
  if (_Color.Alpha <= 0.0f)
  {
    return;
  }

  if (pStaticMeshCameraData_->isChange)
  {
    pStaticMeshCameraData_->isChange = false;
    pStaticMeshCameraData_->ConstantBuffer.GetData().ViewProjection = GetView(RenderType::eStaticMesh) * GetProjection(RenderType::eStaticMesh);
    StaticMeshRendererCommandList_.PushVertexStageConstantBuffer(pStaticMeshCameraData_->ConstantBuffer.GetDetail(), 0);
  }

  StaticMeshInstanceData InstanceData;
  {
    InstanceData.World = _World * _StaticMesh.GetGlobalTransform();
    InstanceData.Color = _Color;
  }
  StaticMeshRendererCommandList_.PushDrawData(_StaticMesh, std::move(InstanceData));
}

void CRenderer::Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const vdl::MotionBlendDatas& _MotionBlendDatas, const vdl::ColorF& _Color)
{
  //  �\������Ȃ����̂͏���
  if (_Color.Alpha <= 0.0f)
  {
    return;
  }

  if (pSkinnedMeshCameraData_->isChange)
  {
    pSkinnedMeshCameraData_->isChange = false;
    pSkinnedMeshCameraData_->ConstantBuffer.GetData().ViewProjection = GetView(RenderType::eStaticMesh) * GetProjection(RenderType::eStaticMesh);
    SkinnedMeshRendererCommandList_.PushVertexStageConstantBuffer(pSkinnedMeshCameraData_->ConstantBuffer.GetDetail(), 0);
  }

  SkinnedMeshinstanceData InstanceData;
  {
    InstanceData.World = _World * _SkinnedMesh.GetGlobalTransform();
    InstanceData.Color = _Color;
  }
  SkinnedMeshRendererCommandList_.PushDrawData(_SkinnedMesh, std::move(InstanceData));
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
  const bool HasStaticMeshDrawCommand = StaticMeshRendererCommandList_.HasDrawCommand();
  const bool HasSkinnedMeshDrawCommand = SkinnedMeshRendererCommandList_.HasDrawCommand();
  const bool HasTextureDrawCommand = TextureRendererCommandList_.HasDrawCommand();

  if (HasEmptyDrawCommand || HasStaticMeshDrawCommand || HasSkinnedMeshDrawCommand || HasTextureDrawCommand)
  {
    //  ���ꂼ��̃h���[�R�[�����\�[�g(�}���`�X���b�h)
    {
      if (HasEmptyDrawCommand)
      {
        EmptyRendererCommandList_.Adjust();
      }
      if (HasStaticMeshDrawCommand)
      {
        StaticMeshRendererCommandList_.Adjust();
      }
      //if (HasSkinnedMeshDrawCommand)
      //{
      //  SkinnedMeshRendererCommandList_.Adjust();
      //}
      if (HasTextureDrawCommand)
      {
        TextureRendererCommandList_.Adjust();
      }
    }

    pDeviceContext_->SetRenderTextures(OutputManager_.RenderTextures, OutputManager_.DepthStencilTexture);

    if (HasStaticMeshDrawCommand)
    {
      pDeviceContext_->SetInputLayout(vdl::InputLayoutType::eStaticMesh);
      StaticMeshRendererCommandList_.Flush(pDeviceContext_, pStaticMeshInstanceBuffer_.get());
      StaticMeshRendererCommandList_.Reset();
    }
    //if (HasSkinnedMeshDrawCommand)
    //{
    //  pDeviceContext_->SetInputLayout(vdl::InputLayout::eSkinnedMesh);
    //  SkinnedMeshRendererCommandList_.Flush(pDeviceContext_, pSkinnedMeshInstanceBuffer_.get());
    //  SkinnedMeshRendererCommandList_.Reset();
    //}
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

      vdl::Texture EmptyTexture;
      pDeviceContext_->PSSetTextures(0, 1, &EmptyTexture);
    }

    pDeviceContext_->Flush();
  }
}
