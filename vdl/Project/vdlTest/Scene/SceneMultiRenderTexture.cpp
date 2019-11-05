#include "SceneMultiRenderTexture.hpp"

using namespace vdl;

void SceneMultiRenderTexture::Initialize()
{
  for (uint RenderTextureCount = 0; RenderTextureCount < kUsingRenderTextureNum; ++RenderTextureCount)
  {
    RenderTextures_[RenderTextureCount] = vdl::RenderTexture(kRenderTextureSize, FormatType::eSwapChain);
  }

  Model_ = vdl::Model("Data/danbo_atk.fbx");
  Camera_ = Camera(vdl::float3(0.0f, 0.0f, -15.0f));

  Renderer3D::SetVertexShader(VertexShader("Shader/MultiRenderTexture/MultiRenderTextureVS.hlsl", InputLayoutType::eMesh));
  Renderer3D::SetPixelShader(PixelShader("Shader/MultiRenderTexture/MultiRenderTexturePS.hlsl"));
}

void SceneMultiRenderTexture::Update()
{
  for (uint RenderTextureCount = 0; RenderTextureCount < kUsingRenderTextureNum; ++RenderTextureCount)
  {
    Renderer::Clear(RenderTextures_[RenderTextureCount]);
  }

  FreeCamera(&Camera_);
  Renderer3D::SetCamera(Camera_);

  Renderer::SetRenderTextures(RenderTextures_, Window::GetDepthStencilTexture());

  Renderer3D::Draw(Model_, Matrix::Identity());

  Renderer::SetRenderTexture(Window::GetRenderTexture(), Window::GetDepthStencilTexture());

  for (uint RenderTextureCount = 0; RenderTextureCount < kUsingRenderTextureNum; ++RenderTextureCount)
  {
    Renderer2D::Draw(RenderTextures_[RenderTextureCount],
      float2(kRenderTextureSize.x / kUsingRenderTextureNum, 0.0f) * RenderTextureCount,
      float2(kRenderTextureSize.x / kUsingRenderTextureNum, kRenderTextureSize.y));
  }
}
