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
    TextureRendererCommandList_.Initialize(vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault2D, vdl::RasterizerState::kDefault2D, vdl::Sampler::kDefault2D,
      vdl::VertexShader(Constants::kDefaultTextureVertexShaderCode, Constants::kDefaultTextureVertexShaderSize, vdl::InputLayout::eTexture),
      vdl::PixelShader(Constants::kDefaultTexturePixelShaderCode, Constants::kDefaultTexturePixelShaderSize));
    StaticMeshRendererCommandList_.Initialize(vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D, vdl::Sampler::kDefault3D,
      vdl::VertexShader(Constants::kDefaultStaticMeshVertexShaderCode, Constants::kDefaultStaticMeshVertexShaderSize, vdl::InputLayout::eStaticMesh),
      vdl::PixelShader(Constants::kDefaultStaticMeshPixelShaderCode, Constants::kDefaultStaticMeshPixelShaderSize));
    SkinnedMeshRendererCommandList_.Initialize(vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D, vdl::Sampler::kDefault3D,
      vdl::VertexShader(Constants::kDefaultSkinnedMeshVertexShaderCode, Constants::kDefaultSkinnedMeshVertexShaderSize, vdl::InputLayout::eSkinnedMesh),
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

void CRenderer::Flush()
{
  const bool HasStaticMeshDrawCommand = StaticMeshRendererCommandList_.HasDrawCommand();
  const bool HasSkinnedMeshDrawCommand = SkinnedMeshRendererCommandList_.HasDrawCommand();
  const bool HasTextureDrawCommand = TextureRendererCommandList_.HasDrawCommand();

  if (HasStaticMeshDrawCommand || HasSkinnedMeshDrawCommand || HasTextureDrawCommand)
  {
    //  ���ꂼ��̃h���[�R�[�����\�[�g(�}���`�X���b�h)
    {
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
      pDeviceContext_->SetInputLayout(vdl::InputLayout::eStaticMesh);
      pDeviceContext_->SetTopology(vdl::Topology::eTriangleList);
      StaticMeshRendererCommandList_.Flush(pDeviceContext_, pStaticMeshInstanceBuffer_.get());
      StaticMeshRendererCommandList_.Reset();
    }
    //if (HasSkinnedMeshDrawCommand)
    //{
    //  pDeviceContext_->SetInputLayout(vdl::InputLayout::eSkinnedMesh);
    //  SkinnedMeshRendererCommandList_.Flush(pDeviceContext_, pSkinnedMeshInstanceBuffer_.get());
    //  SkinnedMeshRendererCommandList_.Reset();
    //}
    if (HasTextureDrawCommand)
    {
      pDeviceContext_->SetInputLayout(vdl::InputLayout::eTexture);
      pDeviceContext_->SetTopology(vdl::Topology::eTriangleStrip);
      pDeviceContext_->SetVertexBuffer(pTextureVertexBuffer_.get());
      TextureRendererCommandList_.Flush(pDeviceContext_, pTextureInstanceBuffer_.get());
      TextureRendererCommandList_.Reset();

      vdl::Texture EmptyTexture;
      pDeviceContext_->PSSetTextures(0, 1, &EmptyTexture);
    }

    pDeviceContext_->Flush();
  }
}
