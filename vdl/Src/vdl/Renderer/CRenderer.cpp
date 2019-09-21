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

void CRenderer::Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const std::vector<vdl::MotionBlendData>& _MotionBlendDatas, const vdl::ColorF& _Color)
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

}

void CRenderer::Clear(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)
{

}

void CRenderer::Flush()
{

}

//void CRenderer::Initialize()
//{
//  pDevice_ = Engine::Get<IDevice>();
//  pDeviceContext_ = Engine::Get<IDeviceContext>();
//
//  //  バッファの作成
//  {
//    IBuffer* pVertexBuffer2D = pVertexBuffer2D_.get();
//    pDevice_->CreateVertexBuffer(&pVertexBuffer2D, kRectangle, sizeof(vdl::Vertex2D), static_cast<vdl::uint>(sizeof(vdl::Vertex2D) * vdl::Macro::ArraySize(kRectangle)));
//
//    IBuffer* pInstanceBuffer2D = pInstanceBuffer2D_.get();
//    pDevice_->CreateInstanceBuffer(&pInstanceBuffer2D, sizeof(Instance2D), sizeof(Instance2D) * Constants::kMax2DBatchNum);
//
//    IBuffer* pInstanceBuffer3D = pInstanceBuffer3D_.get();
//    pDevice_->CreateInstanceBuffer(&pInstanceBuffer3D, sizeof(Instance3D), sizeof(Instance3D) * Constants::kMax3DBatchNum);
//  }
//
//  //  シェーダーの作成
//  {
//    Shaders_[static_cast<vdl::uint>(RenderType::eSprite)].VertexShader = vdl::VertexShader(Constants::kDefaultTextureVertexShaderFilePath, vdl::InputLayout::e2D);
//    Shaders_[static_cast<vdl::uint>(RenderType::eSprite)].PixelShader = vdl::PixelShader(Constants::kDefaultTexturePixelShaderFilePath);
//
//    Shaders_[static_cast<vdl::uint>(RenderType::eStaticMesh)].VertexShader = vdl::VertexShader(Constants::kDefaultStaticMeshVertexShaderFilePath, vdl::InputLayout::e3D);
//    Shaders_[static_cast<vdl::uint>(RenderType::eStaticMesh)].PixelShader = vdl::PixelShader(Constants::kDefaultStaticMeshPixelShaderFilePath);
//
//    Shaders_[static_cast<vdl::uint>(RenderType::eSkinnedMesh)].VertexShader = vdl::VertexShader(Constants::kDefaultSkinnedMeshVertexShaderFilePath, vdl::InputLayout::e3D);
//    Shaders_[static_cast<vdl::uint>(RenderType::eSkinnedMesh)].PixelShader = vdl::PixelShader(Constants::kDefaultSkinnedMeshPixelShaderFilePath);
//  }
//
//  //  サンプラーの作成
//  {
//    SetSamplers(0, 1, &vdl::Sampler(vdl::Sampler::kDefault2D), ShaderType::ePixelShader, RenderType::eSprite);
//    SetSamplers(0, 1, &vdl::Sampler(vdl::Sampler::kDefault3D), ShaderType::ePixelShader, RenderType::eStaticMesh);
//    SetSamplers(0, 1, &vdl::Sampler(vdl::Sampler::kDefault3D), ShaderType::ePixelShader, RenderType::eSkinnedMesh);
//  }
//
//  //  ステートの設定
//  {
//    GraphicsStates_[static_cast<vdl::uint>(RenderType::eSprite)].BlendState = vdl::BlendState::kDefault;
//    GraphicsStates_[static_cast<vdl::uint>(RenderType::eSprite)].DepthSteniclState = vdl::DepthStencilState::kDefault2D;
//    GraphicsStates_[static_cast<vdl::uint>(RenderType::eSprite)].RasterizerState = vdl::RasterizerState::kDefault2D;
//
//    GraphicsStates_[static_cast<vdl::uint>(RenderType::eStaticMesh)].BlendState = GraphicsStates_[static_cast<vdl::uint>(RenderType::eSkinnedMesh)].BlendState = vdl::BlendState::kDefault;
//    GraphicsStates_[static_cast<vdl::uint>(RenderType::eStaticMesh)].DepthSteniclState = GraphicsStates_[static_cast<vdl::uint>(RenderType::eSkinnedMesh)].DepthSteniclState = vdl::DepthStencilState::kDefault3D;
//    GraphicsStates_[static_cast<vdl::uint>(RenderType::eStaticMesh)].RasterizerState = GraphicsStates_[static_cast<vdl::uint>(RenderType::eSkinnedMesh)].RasterizerState = vdl::RasterizerState::kDefault3D;
//  }
//}
//
//void CRenderer::Draw(const vdl::Texture& _Texture, const vdl::float2& _DstLeftTop, const vdl::float2& _DstSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::ColorF& _Color)
//{
//  ////  表示されないものは除く
//  //if (_Color.A <= 0.0f || _DstSize.x * _DstSize.y == 0.0f)
//  //{
//  //  return;
//  //}
//
//  Instance2D Instance;
//  {
//    Instance.NDCTransform = vdl::Matrix(DirectX::XMMatrixAffineTransformation2D(DirectX::XMVectorSet(_DstSize.x, _DstSize.y, 0.0f, 0.0f), DirectX::XMVectorZero(),
//      _Angle, DirectX::XMVectorSet(_DstLeftTop.x + _DstSize.x * 0.5f, _DstLeftTop.y + _DstSize.y * 0.5f, 0.0f, 0.0f))
//      * vdl::Matrix::Scale({ 2.0f / Viewport_.Size.x, -2.0f / Viewport_.Size.y, 1.0f }) * vdl::Matrix::Translate({ -1.0f, 1.0f, 0.0f }));
//
//    const vdl::int2& Size = Textures_[static_cast<vdl::uint>(RenderType::eSprite)][static_cast<vdl::uint>(ShaderType::ePixelShader)][0].GetSize();
//
//    Instance.TexcoordScale = _SrcSize / Size;
//    Instance.TexcoordTranslate = _SrcLeftPos / Size;
//
//    Instance.Color = _Color;
//  }
//
//  Instances2D_.emplace_back(std::move(Instance));
//
//  if (Instances2D_.size() >= Constants::kMax2DBatchNum)
//  {
//    FlushSprite();
//  }
//}
//
//void CRenderer::Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::ColorF& _Color)
//{
//  ////  表示されないものは除く
//  //if (_Color.A <= 0.0f)
//  //{
//  //  return;
//  //}
//
//  Instance3D Instance;
//  {
//    Instance.World = _World;
//    Instance.MaterialColor = _Color;
//  }
//
//  Instances3D_.emplace_back(std::move(Instance));
//
//  if (Instances3D_.size() >= Constants::kMax3DBatchNum)
//  {
//    Flush();
//  }
//}
//
//void CRenderer::Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const std::vector<vdl::MotionBlendData>& _MotionBlendDatas, const vdl::ColorF& _Color)
//{
//  ////  表示されないものは除く
//  //if (_Color.A <= 0.0f)
//  //{
//  //  return;
//  //}
//
//  Instance3D Instance;
//  {
//    Instance.World = _World;
//    Instance.MaterialColor = _Color;
//  }
//
//  Instances3D_.emplace_back(std::move(Instance));
//
//  if (Instances3D_.size() >= Constants::kMax3DBatchNum)
//  {
//    Flush();
//  }
//}
//
//void CRenderer::Clear(const vdl::RenderTexture& _RenderTexture, const vdl::ColorF& _ClearColor)override
//{
//
//}
//
//void CRenderer::Clear(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil)override
//{
//
//}
//
//void CRenderer::Flush()
//{
//  FlushStaticMesh();
//
//  FlushSkinnedMesh();
//
//  FlushSprite();
//}
//
////--------------------------------------------------
//
//void CRenderer::SetGraphicsStateAndOutputManager(vdl::InputLayout _InputLayout)
//{
//  pDeviceContext_->SetInputLayout(_InputLayout);
//
//  pDeviceContext_->SetScissor(Scissor_);
//  pDeviceContext_->SetViewport(Viewport_);
//  pDeviceContext_->SetRenderTexture(OutputManager_.RenderTexture, OutputManager_.DepthStencilTexture);
//
//  pDeviceContext_->SetBlendState(GraphicsStates_[static_cast<vdl::uint>(_InputLayout)].BlendState);
//  pDeviceContext_->SetDepthStencilState(GraphicsStates_[static_cast<vdl::uint>(_InputLayout)].DepthSteniclState);
//  pDeviceContext_->SetRasterizerState(GraphicsStates_[static_cast<vdl::uint>(_InputLayout)].RasterizerState);
//}
//
//void CRenderer::SetRenderState(RenderType _Type)
//{
//  //  VertexShaderStage
//  {
//    const vdl::VertexShader& VertexShader = Shaders_[static_cast<vdl::uint>(_Type)].VertexShader;
//    pDeviceContext_->VSSetShader(VertexShader);
//
//    if (!VertexShader.isEmpty())
//    {
//      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::eVertexShader)];
//      pDeviceContext_->VSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
//      const Textures& Textures = Textures_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::eVertexShader)];
//      pDeviceContext_->VSSetTextures(0, static_cast<vdl::uint>(Textures.size()), Textures.data());
//      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::eVertexShader)];
//      pDeviceContext_->VSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
//    }
//  }
//
//  //  HullShaderStage
//  {
//    const vdl::HullShader& HullShader = Shaders_[static_cast<vdl::uint>(_Type)].HullShader;
//    pDeviceContext_->HSSetShader(HullShader);
//
//    if (!HullShader.isEmpty())
//    {
//      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::eHullShader)];
//      pDeviceContext_->HSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
//      const Textures& Textures = Textures_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::eHullShader)];
//      pDeviceContext_->HSSetTextures(0, static_cast<vdl::uint>(Textures.size()), Textures.data());
//      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::eHullShader)];
//      pDeviceContext_->HSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
//    }
//  }
//
//  //  DomainShaderStage
//  {
//    const vdl::DomainShader& DomainShader = Shaders_[static_cast<vdl::uint>(_Type)].DomainShader;
//    pDeviceContext_->DSSetShader(DomainShader);
//
//    if (!DomainShader.isEmpty())
//    {
//      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::eDomainShader)];
//      pDeviceContext_->DSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
//      const Textures& Textures = Textures_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::eDomainShader)];
//      pDeviceContext_->DSSetTextures(0, static_cast<vdl::uint>(Textures.size()), Textures.data());
//      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::eDomainShader)];
//      pDeviceContext_->DSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
//    }
//  }
//
//  //  GeometryShaderStage
//  {
//    const vdl::GeometryShader& GeometryShader = Shaders_[static_cast<vdl::uint>(_Type)].GeometryShader;
//    pDeviceContext_->GSSetShader(GeometryShader);
//
//    if (!GeometryShader.isEmpty())
//    {
//      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::eGeometryShader)];
//      pDeviceContext_->GSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
//      const Textures& Textures = Textures_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::eGeometryShader)];
//      pDeviceContext_->GSSetTextures(0, static_cast<vdl::uint>(Textures.size()), Textures.data());
//      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::eGeometryShader)];
//      pDeviceContext_->GSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
//    }
//  }
//
//  //  PixelShaderStage
//  {
//    const vdl::PixelShader& PixelShader = Shaders_[static_cast<vdl::uint>(_Type)].PixelShader;
//    pDeviceContext_->PSSetShader(PixelShader);
//
//    if (!PixelShader.isEmpty())
//    {
//      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::ePixelShader)];
//      pDeviceContext_->PSSetSamplers(0, static_cast<vdl::uint>(Samplers.size()), Samplers.data());
//      const Textures& Textures = Textures_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::ePixelShader)];
//      pDeviceContext_->PSSetTextures(0, static_cast<vdl::uint>(Textures.size()), Textures.data());
//      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(_Type)][static_cast<vdl::uint>(ShaderType::ePixelShader)];
//      pDeviceContext_->PSSetConstantBuffers(0, static_cast<vdl::uint>(ConstantBuffers.size()), ConstantBuffers.data());
//    }
//  }
//}
//
//void CRenderer::FlushSprite()
//{
//  assert(!Instances2D_.empty());
//
//  const vdl::uint InstanceNum = static_cast<vdl::uint>(Instances2D_.size());
//  pDevice_->WriteMemory(pInstanceBuffer2D_.get(), Instances2D_.data(), sizeof(vdl::Vertex2D) * InstanceNum);
//
//  pDeviceContext_->SetVertexBuffer(pVertexBuffer2D_.get());
//  pDeviceContext_->SetInstanceBuffer(pInstanceBuffer2D_.get());
//  pDeviceContext_->SetTopology(vdl::Topology::eTriangleStrip);
//
//  SetGraphicsStateAndOutputManager(vdl::InputLayout::e2D);
//  SetRenderState(RenderType::eSprite);
//
//  pDeviceContext_->Draw(4, InstanceNum, 0, 0);
//
//  Instances2D_.clear();
//}
//
//void CRenderer::FlushStaticMesh()
//{
//  SetGraphicsStateAndOutputManager(vdl::InputLayout::e2D);
//  SetRenderState(RenderType::eStaticMesh);
//
//  Instances3D_.clear();
//}
//
//void CRenderer::FlushSkinnedMesh()
//{
//  SetGraphicsStateAndOutputManager(vdl::InputLayout::e2D);
//  SetRenderState(RenderType::eSkinnedMesh);
//
//  Instances3D_.clear();
//}
