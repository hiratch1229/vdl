#include "CRenderer.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>
#include <vdl/ShaderManager/IShaderManager.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vdl/Topology.hpp>
#include <vdl/Macro.hpp>

void CRenderer::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
  pDeviceContext_ = Engine::Get<IDeviceContext>();

  StaticMeshCameraData_.Camera = SkinnedMeshCameraData_.Camera = vdl::Camera({ 0.0f,0.0f,-10.0f });
  StaticMeshCameraData_.isChange = SkinnedMeshCameraData_.isChange = true;

  //  バッファの作成
  {
    IBuffer* pTextureVertexBuffer = pTextureVertexBuffer_.get();
    pDevice_->CreateVertexBuffer(&pTextureVertexBuffer, kRectangle, sizeof(vdl::TextureVertex), static_cast<vdl::uint>(sizeof(vdl::TextureVertex) * vdl::Macro::ArraySize(kRectangle)));

    IBuffer* pTextureInstanceBuffer = pTextureInstanceBuffer_.get();
    pDevice_->CreateInstanceBuffer(&pTextureInstanceBuffer, sizeof(TextureInstanceData), sizeof(TextureInstanceData) * Constants::kMaxTextureBatchNum);

    IBuffer* pStaticMeshInstanceBuffer = pStaticMeshInstanceBuffer_.get();
    pDevice_->CreateInstanceBuffer(&pStaticMeshInstanceBuffer, sizeof(StaticMeshInstanceData), sizeof(StaticMeshInstanceData) * Constants::kMaxStaticMeshBatchNum);

    IBuffer* pSkinnedMeshInstanceBuffer = pSkinnedMeshInstanceBuffer_.get();
    pDevice_->CreateInstanceBuffer(&pSkinnedMeshInstanceBuffer, sizeof(SkinnedMeshinstanceData), sizeof(SkinnedMeshinstanceData) * Constants::kMaxSkinnedMeshBatchNum);
  }

  //  描画コマンドリストの初期化
  {
    const vdl::uint2 WindowSize = std::move(Engine::Get<IWindow>()->GetWindowSize());

    const vdl::Scissor Scissor(0, WindowSize);
    const vdl::Viewport Viewport(0, WindowSize);

    TextureRendererCommandList_.Initialize(Scissor, Viewport, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault2D, vdl::RasterizerState::kDefault2D,
      vdl::Sampler::kDefault2D, vdl::VertexShader(Constants::kDefaultTextureVertexShaderFilePath, vdl::InputLayout::eTexture), vdl::PixelShader(Constants::kDefaultTexturePixelShaderFilePath));
    StaticMeshRendererCommandList_.Initialize(Scissor, Viewport, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D,
      vdl::Sampler::kDefault3D, vdl::VertexShader(Constants::kDefaultStaticMeshVertexShaderFilePath, vdl::InputLayout::eStaticMesh), vdl::PixelShader(Constants::kDefaultStaticMeshPixelShaderFilePath));
    SkinnedMeshRendererCommandList_.Initialize(Scissor, Viewport, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D,
      vdl::Sampler::kDefault3D, vdl::VertexShader(Constants::kDefaultSkinnedMeshVertexShaderFilePath, vdl::InputLayout::eSkinnedMesh), vdl::PixelShader(Constants::kDefaultSkinnedMeshPixelShaderFilePath));
  }
}

vdl::Matrix CRenderer::GetView(RenderType _Type)const
{
  const vdl::Camera& Camera = (_Type == RenderType::eStaticMesh ? StaticMeshCameraData_.Camera : SkinnedMeshCameraData_.Camera);

  return DirectX::XMMatrixLookAtLH({ Camera.Position.x, Camera.Position.y, Camera.Position.z, 1.0f },
    { Camera.Target.x, Camera.Target.y, Camera.Target.z, 1.0f }, { Camera.Up.x, Camera.Up.y, Camera.Up.z, 0.0f });
}

vdl::Matrix CRenderer::GetProjection(RenderType _Type)const
{
  const vdl::Camera& Camera = (_Type == RenderType::eStaticMesh ? StaticMeshCameraData_.Camera : SkinnedMeshCameraData_.Camera);
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
  CameraData& CameraData = (_Type == RenderType::eStaticMesh ? StaticMeshCameraData_ : SkinnedMeshCameraData_);
  {
    CameraData.Camera = _Camera;
    CameraData.isChange = true;
  }
}

void CRenderer::SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStencilTexture)
{
  OutputManager OutputManager{ _RenderTextures, _DepthStencilTexture };

  if (OutputManager_ != OutputManager)
  {
    Flush();
    OutputManager = std::move(OutputManager);
  }
}

void CRenderer::Draw(const vdl::Texture& _Texture, const vdl::float2& _DestLeftTop, const vdl::float2& _DestSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::ColorF& _Color)
{
  //  表示されないものは除く
  if (_Color.Alpha <= 0.0f || _DestSize.x * _DestSize.y == 0.0f)
  {
    return;
  }

  TextureInstanceData InstanceData;
  {
    const vdl::Viewport& Viewport = TextureRendererCommandList_.GetCurrentViewport();

    InstanceData.NDCTransform = std::move(vdl::Matrix(DirectX::XMMatrixAffineTransformation2D(DirectX::XMVectorSet(_DestSize.x, _DestSize.y, 0.0f, 0.0f), DirectX::XMVectorZero(),
      _Angle, DirectX::XMVectorSet(_DestLeftTop.x + _DestSize.x * 0.5f, _DestLeftTop.y + _DestSize.y * 0.5f, 0.0f, 0.0f))
      * vdl::Matrix::Scale({ 2.0f / Viewport.Size.x, -2.0f / Viewport.Size.y, 1.0f }) * vdl::Matrix::Translate({ -1.0f, 1.0f, 0.0f })));

    const vdl::uint2 TextureSize = std::move(_Texture.GetSize());

    InstanceData.TexcoordScale = _SrcSize / TextureSize;
    InstanceData.TexcoordTranslate = _SrcLeftPos / TextureSize;

    InstanceData.Color = _Color;
  }
  TextureRendererCommandList_.PushDrawData(_Texture, std::move(InstanceData));
}

void CRenderer::Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::ColorF& _Color)
{
  //  表示されないものは除く
  if (_Color.Alpha <= 0.0f)
  {
    return;
  }

  if (StaticMeshCameraData_.isChange)
  {
    StaticMeshCameraData_.isChange = false;
    StaticMeshCameraData_.ConstantBuffer.GetData().ViewProjection = GetView(RenderType::eStaticMesh) * GetProjection(RenderType::eStaticMesh);
    StaticMeshRendererCommandList_.PushVertexStageConstantBuffer(StaticMeshCameraData_.ConstantBuffer.GetDetail(), 0);
  }

  StaticMeshInstanceData InstanceData;
  {
    InstanceData.World = std::move(_World * _StaticMesh.GetGlobalTransform());
    InstanceData.Color = _Color;
  }
  StaticMeshRendererCommandList_.PushDrawData(_StaticMesh, std::move(InstanceData));
}

void CRenderer::Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const vdl::MotionBlendDatas& _MotionBlendDatas, const vdl::ColorF& _Color)
{
  //  表示されないものは除く
  if (_Color.Alpha <= 0.0f)
  {
    return;
  }

  if (SkinnedMeshCameraData_.isChange)
  {
    SkinnedMeshCameraData_.isChange = false;
    SkinnedMeshCameraData_.ConstantBuffer.GetData().ViewProjection = GetView(RenderType::eStaticMesh) * GetProjection(RenderType::eStaticMesh);
    SkinnedMeshRendererCommandList_.PushVertexStageConstantBuffer(SkinnedMeshCameraData_.ConstantBuffer.GetDetail(), 0);
  }

  SkinnedMeshinstanceData InstanceData;
  {
    InstanceData.World = std::move(_World * _SkinnedMesh.GetGlobalTransform());
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
  //  それぞれのドローコールをソート(マルチスレッド)
  {
    StaticMeshRendererCommandList_.Adjust();
    SkinnedMeshRendererCommandList_.Adjust();
    TextureRendererCommandList_.Adjust();
  }

  pDeviceContext_->SetRenderTextures(OutputManager_.RenderTextures, OutputManager_.DepthStencilTexture);

  pDeviceContext_->SetInputLayout(vdl::InputLayout::eStaticMesh);
  pDeviceContext_->SetTopology(vdl::Topology::eTriangleStrip);
  StaticMeshRendererCommandList_.Flush(pDeviceContext_, pStaticMeshInstanceBuffer_.get());

  pDeviceContext_->SetInputLayout(vdl::InputLayout::eSkinnedMesh);
  SkinnedMeshRendererCommandList_.Flush(pDeviceContext_, pSkinnedMeshInstanceBuffer_.get());

  pDeviceContext_->SetInputLayout(vdl::InputLayout::eTexture);
  pDeviceContext_->SetTopology(vdl::Topology::eTriangleStrip);
  pDeviceContext_->SetVertexBuffer(pTextureVertexBuffer_.get());
  TextureRendererCommandList_.Flush(pDeviceContext_, pTextureInstanceBuffer_.get());

  StaticMeshRendererCommandList_.Reset();
  SkinnedMeshRendererCommandList_.Reset();
  TextureRendererCommandList_.Reset();

  pDeviceContext_->Flush();
}
