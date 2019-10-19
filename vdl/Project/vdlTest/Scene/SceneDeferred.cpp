#include "SceneDeferred.hpp"

using namespace vdl;

namespace
{
  float QuadraticFunction(float _Time, float _TotalTime, float _Min, float _Max)
  {
    return (1.0f - (_Time / (_TotalTime / 2.0f) - 1.0f) * (_Time / (_TotalTime / 2.0f) - 1.0f)) * (_Max - _Min) + _Min;
  }
}

void SceneDeferred::Initialize()
{
  Rectangle_ = MeshData::Rectangle("Data/test1_albedo.png", "Data/test1_normal.png");
  Sphere_ = MeshData::Sphere(8, 8, "Data/earthmap.jpg", "Data/earthnormal.jpg");
  Camera_ = Camera(float3(0.0f, 5.0f, -15.0f));

  for (auto& Data : Datas_)
  {
    Data.Color = ColorF(Random::Range(-kRectangleHalfScale, kRectangleHalfScale), Random::Range(-kRectangleHalfScale, kRectangleHalfScale), Random::Range(-kRectangleHalfScale, kRectangleHalfScale));
    Data.MinRange.x = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
    Data.MinRange.z = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
    Data.MaxRange.x = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
    Data.MaxRange.z = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
    Data.Timer = 0.0f;
    Data.Time = Random::Range(5.0f, 15.0f);
  }

  const uint2 WindowSize = Window::GetWindowSize();
  DeferredRenderTextures_[0] = RenderTexture(WindowSize, FormatType::eR8G8B8A8_Unorm);
  DeferredRenderTextures_[1] = RenderTexture(WindowSize, FormatType::eR32G32B32A32_Float);
  DeferredDepthTexture_ = DepthStencilTexture(WindowSize, FormatType::eD16_Unorm);
}

SceneDeferred::~SceneDeferred()
{

}

void SceneDeferred::Update()
{
  FreeCamera(&Camera_);

  Renderer3D::SetCamera(Camera_);

  const float DeltaTime = System::GetDeltaTime();
  for (auto& Data : Datas_)
  {
    Data.Timer += DeltaTime;
    while (Data.Timer > Data.Time)
    {
      Data.Timer -= Data.Time;
    }
  }

  for (vdl::uint i = 0; i < kUseRenderTextureNum; ++i)
  {
    Renderer::Clear(DeferredRenderTextures_[i]);
  }
  Renderer::Clear(DeferredDepthTexture_);
  Renderer::SetRenderTextures(DeferredRenderTextures_, DeferredDepthTexture_);
  {
    GBufferPassVertexShader_ = VertexShader("Shader/Deferred/GBufferPassVS.hlsl", InputLayoutType::eMesh);
    GBufferPassPixelShader_ = PixelShader("Shader/Deferred/GBufferPassPS.hlsl");
    Renderer3D::SetShaders(GBufferPassVertexShader_, GBufferPassPixelShader_);

    Renderer3D::Draw(Rectangle_, Matrix::Scale(kRectangleScale) * Matrix::Rotate(Math::ToRadian(90.0f), 0.0f, 0.0f));
    for (auto& Data : Datas_)
    {
      Renderer3D::Draw(Sphere_, Matrix::Translate(float3(QuadraticFunction(Data.Timer, Data.Time, Data.MinRange.x, Data.MaxRange.x), 0.5f, QuadraticFunction(Data.Timer, Data.Time, Data.MinRange.z, Data.MaxRange.z))));
    }


  }
  Renderer::SetRenderTexture(RenderTexture(), DepthStencilTexture());

  const uint2 WindowHalfSize = Window::GetWindowSize() / 2;

  for (vdl::uint i = 0; i < kUseRenderTextureNum; ++i)
  {
    Renderer2D::Draw(DeferredRenderTextures_[i], float2(WindowHalfSize.x * i, 0.0f), WindowHalfSize);
  }
  Renderer2D::Draw(DeferredDepthTexture_, float2(0.0f, WindowHalfSize.y), WindowHalfSize);
}
