#include "SceneMultiRenderTexture.hpp"

using namespace vdl;

void SceneMultiRenderTexture::Initialize()
{
  for (uint RenderTextureCount = 0; RenderTextureCount < kUsingRenderTextureNum; ++RenderTextureCount)
  {
    RenderTextures_[RenderTextureCount] = vdl::RenderTexture(kRenderTextureSize, FormatType::eSwapChain);
  }

  Model_ = vdl::Model("Data/danbo_atk.fbx");

  Renderer3D::SetVertexShader(VertexShader("Shader/MultiRenderTexture/MultiRenderTextureVS.hlsl", InputLayoutType::eMesh));
  Renderer3D::SetPixelShader(PixelShader("Shader/MultiRenderTexture/MultiRenderTexturePS.hlsl"));
}

SceneMultiRenderTexture::~SceneMultiRenderTexture()
{
  Renderer3D::SetShaders(VertexShader("Shader/Mesh/MeshVS.hlsl", InputLayoutType::eMesh),
    PixelShader("Shader/Mesh/MeshPS.hlsl"));
}

void SceneMultiRenderTexture::Update()
{
  for (uint RenderTextureCount = 0; RenderTextureCount < kUsingRenderTextureNum; ++RenderTextureCount)
  {
    Renderer::Clear(RenderTextures_[RenderTextureCount]);
  }

  Renderer::SetRenderTextures(RenderTextures_, DepthStencilTexture());

  Renderer3D::Draw(Model_, Matrix::Identity());

  Renderer::SetRenderTexture(RenderTexture(), DepthStencilTexture());

  for (uint RenderTextureCount = 0; RenderTextureCount < kUsingRenderTextureNum; ++RenderTextureCount)
  {
    Renderer2D::Draw(RenderTextures_[RenderTextureCount],
      float2(kRenderTextureSize.x / kUsingRenderTextureNum, 0.0f) * RenderTextureCount,
      float2(kRenderTextureSize.x / kUsingRenderTextureNum, kRenderTextureSize.y));
  }
}
