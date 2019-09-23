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

  //  バッファの作成
  {
    IBuffer* pTextureVertexBuffer = pTextureVertexBuffer_.get();
    pDevice_->CreateVertexBuffer(&pTextureVertexBuffer, kRectangle, sizeof(vdl::Vertex2D), static_cast<vdl::uint>(sizeof(vdl::Vertex2D) * vdl::Macro::ArraySize(kRectangle)));

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
      vdl::Sampler::kDefault2D, vdl::VertexShader(Constants::kDefaultTextureVertexShaderFilePath, vdl::InputLayout::e2D), vdl::PixelShader(Constants::kDefaultTexturePixelShaderFilePath));
    StaticMeshRendererCommandList_.Initialize(Scissor, Viewport, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D,
      vdl::Sampler::kDefault3D, vdl::VertexShader(Constants::kDefaultStaticMeshVertexShaderFilePath, vdl::InputLayout::e3D), vdl::PixelShader(Constants::kDefaultStaticMeshPixelShaderFilePath));
    SkinnedMeshRendererCommandList_.Initialize(Scissor, Viewport, vdl::BlendState::kDefault, vdl::DepthStencilState::kDefault3D, vdl::RasterizerState::kDefault3D,
      vdl::Sampler::kDefault3D, vdl::VertexShader(Constants::kDefaultSkinnedMeshVertexShaderFilePath, vdl::InputLayout::e3D), vdl::PixelShader(Constants::kDefaultSkinnedMeshPixelShaderFilePath));
  }
}

void CRenderer::SetRenderTextures(const vdl::RenderTexture& _RenderTexture, const vdl::DepthStencilTexture& _DepthStencilTexture)
{
  OutputManager OutputManager{ _RenderTexture, _DepthStencilTexture };

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

  SkinnedMeshinstanceData InstanceData;
  {
    InstanceData.World = std::move(_World * _SkinnedMesh.GetGlobalTransform());
    InstanceData.Color = _Color;
  }
  StaticMeshRendererCommandList_.PushDrawData(_SkinnedMesh, std::move(InstanceData));
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

  pDeviceContext_->SetRenderTexture(OutputManager_.RenderTexture, OutputManager_.DepthStencilTexture);

  pDeviceContext_->SetInputLayout(vdl::InputLayout::e3D);
  pDeviceContext_->SetTopology(vdl::Topology::eTriangleStrip);
  StaticMeshRendererCommandList_.Flush(pDeviceContext_, pStaticMeshInstanceBuffer_.get());

  SkinnedMeshRendererCommandList_.Flush(pDeviceContext_, pSkinnedMeshInstanceBuffer_.get());

  pDeviceContext_->SetInputLayout(vdl::InputLayout::e2D);
  pDeviceContext_->SetTopology(vdl::Topology::eTriangleStrip);
  pDeviceContext_->SetVertexBuffer(pTextureVertexBuffer_.get());
  TextureRendererCommandList_.Flush(pDeviceContext_, pTextureInstanceBuffer_.get());

  StaticMeshRendererCommandList_.Reset();
  SkinnedMeshRendererCommandList_.Reset();
  TextureRendererCommandList_.Reset();

  pDeviceContext_->Flush();
}
