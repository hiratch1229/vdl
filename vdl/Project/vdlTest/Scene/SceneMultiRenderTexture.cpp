#include "SceneMultiRenderTexture.hpp"

SceneMultiRenderTexture::SceneMultiRenderTexture()
{
  for (vdl::uint RenderTextureCount = 0; RenderTextureCount < kUsingRenderTextureNum; ++RenderTextureCount)
  {
    RenderTextures_[RenderTextureCount] = vdl::RenderTexture(kRenderTextureSize, vdl::Constants::kSwapChainFormat);
  }

  Model_ = vdl::Model("Data/danbo_atk.fbx");

  vdl::RendererStaticMesh::SetPixelShader(vdl::PixelShader("Shader/StaticMesh/StaticMeshPS.hlsl"));
}

SceneMultiRenderTexture::~SceneMultiRenderTexture()
{
  vdl::RendererStaticMesh::SetPixelShader(vdl::PixelShader(vdl::Constants::kDefaultStaticMeshPixelShaderCode, static_cast<vdl::uint>(vdl::Macro::ArraySize(vdl::Constants::kDefaultStaticMeshPixelShaderCode))));
}

void SceneMultiRenderTexture::Update()
{
  for (vdl::uint RenderTextureCount = 0; RenderTextureCount < kUsingRenderTextureNum; ++RenderTextureCount)
  {
    vdl::Renderer::Clear(RenderTextures_[RenderTextureCount]);
  }

  vdl::Renderer::SetRenderTextures(RenderTextures_, vdl::DepthStencilTexture());

  vdl::Renderer3D::Draw(Model_, vdl::Matrix::Identity());

  vdl::Renderer::SetRenderTexture(vdl::RenderTexture(), vdl::DepthStencilTexture());

  for (vdl::uint RenderTextureCount = 0; RenderTextureCount < kUsingRenderTextureNum; ++RenderTextureCount)
  {
    vdl::Renderer2D::Draw(RenderTextures_[RenderTextureCount],
      vdl::float2(kRenderTextureSize.x / kUsingRenderTextureNum, 0.0f) * RenderTextureCount,
      vdl::float2(kRenderTextureSize.x / kUsingRenderTextureNum, kRenderTextureSize.y));
  }
}
