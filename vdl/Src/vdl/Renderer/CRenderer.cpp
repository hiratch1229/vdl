#include "CRenderer.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vdl/Topology.hpp>
#include <vdl/Macro.hpp>

void CRenderer::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
  pDeviceContext_ = Engine::Get<IDeviceContext>();

  IBuffer* pVertexBuffer2D = pVertexBuffer2D_.get();
  pDevice_->CreateVertexBuffer(&pVertexBuffer2D, kRectangle, sizeof(vdl::Vertex2D), sizeof(vdl::Vertex2D) * vdl::macro::ArraySize(kRectangle));

  IBuffer* pInstanceBuffer2D = pInstanceBuffer2D_.get();
  pDevice_->CreateInstanceBuffer(&pInstanceBuffer2D, sizeof(Instance2D), sizeof(Instance2D) * Constants::kMax2DBatchNum);

  IBuffer* pInstanceBuffer3D = pInstanceBuffer3D_.get();
  pDevice_->CreateInstanceBuffer(&pInstanceBuffer3D, sizeof(Instance3D), sizeof(Instance3D) * Constants::kMax3DBatchNum);
}

void CRenderer::Draw(const vdl::Texture& _Texture, const vdl::float2& _DstLeftTop, const vdl::float2& _DstSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::ColorF& _Color)
{
  ////  ï\é¶Ç≥ÇÍÇ»Ç¢Ç‡ÇÃÇÕèúÇ≠
  //if (_Color.A <= 0.0f || _DstSize.x * _DstSize.y == 0.0f)
  //{
  //  return;
  //}

  Instance2D Instance;
  {
    Instance.NDCTransform = vdl::Matrix(DirectX::XMMatrixAffineTransformation2D(DirectX::XMVectorSet(_DstSize.x, _DstSize.y, 0.0f, 0.0f), DirectX::XMVectorZero(),
      _Angle, DirectX::XMVectorSet(_DstLeftTop.x + _DstSize.x * 0.5f, _DstLeftTop.y + _DstSize.y * 0.5f, 0.0f, 0.0f))
      * vdl::Matrix::Scale({ 2.0f / Viewports_[static_cast<vdl::uint>(RenderType::eSprite)].Size.x, -2.0f / Viewports_[static_cast<vdl::uint>(RenderType::eSprite)].Size.y, 1.0f }) * vdl::Matrix::Translate({ -1.0f, 1.0f, 0.0f }));

    const vdl::int2& Size = Textures_[static_cast<vdl::uint>(RenderType::eSprite)][static_cast<vdl::uint>(ShaderType::ePixelShader)][0].GetSize();

    Instance.TexcoordScale = _SrcSize / Size;
    Instance.TexcoordTranslate = _SrcLeftPos / Size;

    Instance.Color = _Color;
  }

  Instances2D_.emplace_back(std::move(Instance));

  if (Instances2D_.size() >= Constants::kMax2DBatchNum)
  {
    FlushSprite();
  }
}

void CRenderer::Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::ColorF& _Color)
{
  ////  ï\é¶Ç≥ÇÍÇ»Ç¢Ç‡ÇÃÇÕèúÇ≠
  //if (_Color.A <= 0.0f)
  //{
  //  return;
  //}

  Instance3D Instance;
  {
    Instance.World = _World;
    Instance.MaterialColor = _Color;
  }

  Instances3D_.emplace_back(std::move(Instance));

  if (Instances3D_.size() >= Constants::kMax3DBatchNum)
  {
    Flush();
  }
}

void CRenderer::Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const std::vector<vdl::MotionBlendData>& _MotionBlendDatas, const vdl::ColorF& _Color)
{
  ////  ï\é¶Ç≥ÇÍÇ»Ç¢Ç‡ÇÃÇÕèúÇ≠
  //if (_Color.A <= 0.0f)
  //{
  //  return;
  //}

  Instance3D Instance;
  {
    Instance.World = _World;
    Instance.MaterialColor = _Color;
  }

  Instances3D_.emplace_back(std::move(Instance));

  if (Instances3D_.size() >= Constants::kMax3DBatchNum)
  {
    Flush();
  }
}

void CRenderer::Flush()
{

}

//--------------------------------------------------

void CRenderer::SetState(RenderType _RenderType)
{
  pDeviceContext_->SetScissor(Scissors_[static_cast<vdl::uint>(_RenderType)]);
  pDeviceContext_->SetViewport(Viewports_[static_cast<vdl::uint>(_RenderType)]);

  pDeviceContext_->SetBlendState(GraphicsStates_[static_cast<vdl::uint>(_RenderType)].BlendState);
  pDeviceContext_->SetDepthStencilState(GraphicsStates_[static_cast<vdl::uint>(_RenderType)].DepthSteniclState);
  pDeviceContext_->SetRasterizerState(GraphicsStates_[static_cast<vdl::uint>(_RenderType)].RasterizerState);

  pDeviceContext_->SetRenderTexture(OutputManagers_[static_cast<vdl::uint>(_RenderType)].RenderTexture, OutputManagers_[static_cast<vdl::uint>(_RenderType)].DepthStencilTexture);

  //  VertexShaderStage
  {
    const vdl::VertexShader& VertexShader = Shaders_[static_cast<vdl::uint>(_RenderType)].VertexShader;
    pDeviceContext_->VSSetShader(VertexShader);

    if (!VertexShader.isEmpty())
    {
      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::eVertexShader)];
      pDeviceContext_->VSSetSamplers(0, Samplers.size(), Samplers.data());
      const Textures& Textures = Textures_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::eVertexShader)];
      pDeviceContext_->VSSetTextures(0, Textures.size(), Textures.data());
      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::eVertexShader)];
      pDeviceContext_->VSSetConstantBuffers(0, ConstantBuffers.size(), ConstantBuffers.data());
    }
  }

  //  HullShaderStage
  {
    const vdl::HullShader& HullShader = Shaders_[static_cast<vdl::uint>(_RenderType)].HullShader;
    pDeviceContext_->HSSetShader(HullShader);

    if (!HullShader.isEmpty())
    {
      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::eHullShader)];
      pDeviceContext_->HSSetSamplers(0, Samplers.size(), Samplers.data());
      const Textures& Textures = Textures_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::eHullShader)];
      pDeviceContext_->HSSetTextures(0, Textures.size(), Textures.data());
      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::eHullShader)];
      pDeviceContext_->HSSetConstantBuffers(0, ConstantBuffers.size(), ConstantBuffers.data());
    }
  }

  //  DomainShaderStage
  {
    const vdl::DomainShader& DomainShader = Shaders_[static_cast<vdl::uint>(_RenderType)].DomainShader;
    pDeviceContext_->DSSetShader(DomainShader);

    if (!DomainShader.isEmpty())
    {
      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::eDomainShader)];
      pDeviceContext_->DSSetSamplers(0, Samplers.size(), Samplers.data());
      const Textures& Textures = Textures_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::eDomainShader)];
      pDeviceContext_->DSSetTextures(0, Textures.size(), Textures.data());
      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::eDomainShader)];
      pDeviceContext_->DSSetConstantBuffers(0, ConstantBuffers.size(), ConstantBuffers.data());
    }
  }

  //  GeometryShaderStage
  {
    const vdl::GeometryShader& GeometryShader = Shaders_[static_cast<vdl::uint>(_RenderType)].GeometryShader;
    pDeviceContext_->GSSetShader(GeometryShader);

    if (!GeometryShader.isEmpty())
    {
      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::eGeometryShader)];
      pDeviceContext_->GSSetSamplers(0, Samplers.size(), Samplers.data());
      const Textures& Textures = Textures_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::eGeometryShader)];
      pDeviceContext_->GSSetTextures(0, Textures.size(), Textures.data());
      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::eGeometryShader)];
      pDeviceContext_->GSSetConstantBuffers(0, ConstantBuffers.size(), ConstantBuffers.data());
    }
  }

  //  PixelShaderStage
  {
    const vdl::PixelShader& PixelShader = Shaders_[static_cast<vdl::uint>(_RenderType)].PixelShader;
    pDeviceContext_->PSSetShader(PixelShader);

    if (!PixelShader.isEmpty())
    {
      const Samplers& Samplers = Samplers_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::ePixelShader)];
      pDeviceContext_->PSSetSamplers(0, Samplers.size(), Samplers.data());
      const Textures& Textures = Textures_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::ePixelShader)];
      pDeviceContext_->PSSetTextures(0, Textures.size(), Textures.data());
      const ConstantBuffers& ConstantBuffers = ConstantBuffers_[static_cast<vdl::uint>(_RenderType)][static_cast<vdl::uint>(ShaderType::ePixelShader)];
      pDeviceContext_->PSSetConstantBuffers(0, ConstantBuffers.size(), ConstantBuffers.data());
    }
  }
}

void CRenderer::FlushSprite()
{
  assert(!Instances2D_.empty());

  const vdl::uint InstanceNum = Instances2D_.size();
  pDevice_->WriteMemory(pInstanceBuffer2D_.get(), Instances2D_.data(), sizeof(vdl::Vertex2D) * InstanceNum);

  pDeviceContext_->SetVertexBuffer(pVertexBuffer2D_.get());
  pDeviceContext_->SetInstanceBuffer(pInstanceBuffer2D_.get());
  pDeviceContext_->SetInputLayout(vdl::InputLayout::e2D);
  pDeviceContext_->SetTopology(vdl::Topology::eTriangleStrip);

  SetState(RenderType::eSprite);

  pDeviceContext_->Draw(4, InstanceNum, 0, 0);

  Instances2D_.clear();
}

void CRenderer::FlushStaticMesh()
{
  SetState(RenderType::eStaticMesh);

  Instances3D_.clear();
}

void CRenderer::FlushSkinnedMesh()
{
  SetState(RenderType::eSkinnedMesh);

  Instances3D_.clear();
}
