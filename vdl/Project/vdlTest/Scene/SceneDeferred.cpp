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
  Sphere_ = MeshData::Box(/*8, 8, */"Data/earthmap.jpg", "Data/earthnormal.jpg");
  Camera_ = Camera(float3(0.0f, 5.0f, -15.0f));
  PointLightItensity_ = PointLightRange_ = 1.0f;

  for (auto& Data : Datas_)
  {
    Data.Color = ColorF(Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f));
    Data.MinRange.x = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
    Data.MinRange.z = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
    Data.MaxRange.x = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
    Data.MaxRange.z = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
    Data.Timer = 0.0f;
    Data.Time = Random::Range(5.0f, 15.0f);
  }

  GBufferPassVertexShader_ = VertexShader("Shader/Deferred/GBufferPassVS.hlsl", InputLayoutType::eMesh);
  GBufferPassPixelShader_ = PixelShader("Shader/Deferred/GBufferPassPS.hlsl");
  Renderer3D::SetShaders(GBufferPassVertexShader_, GBufferPassPixelShader_);

  GBufferRenderTextures_[0] = RenderTexture(kWindowSize, FormatType::eR8G8B8A8_Unorm);
  GBufferRenderTextures_[1] = RenderTexture(kWindowSize, FormatType::eR32G32B32A32_Float);
  GBufferDepthTexture_ = DepthStencilTexture(kWindowSize, FormatType::eD32_Float);

  LightPassVertexShader_ = VertexShader("Shader/Deferred/LightPassVS.hlsl", InputLayoutType::eNone);
  LightPassPixelShader_ = PixelShader(kLigthPassPSFilePath);

  for (vdl::uint i = 0; i < kUseRenderTextureNum; ++i)
  {
    PixelStageShaderResources_[i] = GBufferRenderTextures_[i];
  }
  PixelStageShaderResources_[kUseRenderTextureNum] = GBufferDepthTexture_;

  Renderer::SetShaders(LightPassVertexShader_, LightPassPixelShader_);
  LightConstantBuffer_.GetData().DirectionalLight = { Camera_.ViewVector(), 1.0f, vdl::Palette::White };
  Renderer::SetPixelStageConstantBuffers(0, 1, &LightConstantBuffer_);
  Renderer::SetPixelStageConstantBuffers(1, 1, &RenderingConstantBuffer_);
  Renderer::SetTopology(TopologyType::eTriangleStrip);
}

SceneDeferred::~SceneDeferred()
{
  Renderer3D::SetShaders(VertexShader("Shader/Mesh/MeshVS.hlsl", InputLayoutType::eMesh),
    PixelShader("Shader/Mesh/MeshPS.hlsl"));
  Renderer::SetTopology(TopologyType::eDefaultNone);
}

void SceneDeferred::Update()
{
  //  î•ñ‚ÌXV
  {
    FreeCamera(&Camera_);

    Renderer3D::SetCamera(Camera_);

    LightData& LightData = LightConstantBuffer_.GetData();

    ImGui::Begin("SceneDeferred");
    {
      if (ImGui::Button("Reload LightPassPS"))
      {
        LightPassPixelShader_ = PixelShader(kLigthPassPSFilePath);
        Renderer::SetPixelShader(LightPassPixelShader_);
      }
      if (ImGui::TreeNode("DirectionalLight"))
      {
        ImGui::InputFloat3("Direction", &LightData.DirectionalLight.Direction.x);
        ImGui::InputFloat("Itensity", &LightData.DirectionalLight.Itensity);
        ImGui::ColorEdit3("Color", &LightData.DirectionalLight.Color.Red);
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("PointLight"))
      {
        ImGui::InputFloat("Itensity", &PointLightItensity_);
        ImGui::InputFloat("Range", &PointLightRange_);
        ImGui::TreePop();
      }
    }
    ImGui::End();

    const float DeltaTime = System::GetDeltaTime();
    for (uint i = 0; i < kDataNum; ++i)
    {
      Data& Data = Datas_[i];

      Data.Timer += DeltaTime;
      while (Data.Timer > Data.Time)
      {
        Data.Timer -= Data.Time;
      }
      Data.Position = float3(QuadraticFunction(Data.Timer, Data.Time, Data.MinRange.x, Data.MaxRange.x), 0.5f, QuadraticFunction(Data.Timer, Data.Time, Data.MinRange.z, Data.MaxRange.z));

      PointLight& PointLight = LightData.PointLights[i];
      PointLight.Position = Data.Position;
      PointLight.Color = Data.Color;
      PointLight.Itensity = PointLightItensity_;
      PointLight.Range = PointLightRange_;
    }
  }

  for (vdl::uint i = 0; i < kUseRenderTextureNum; ++i)
  {
    Renderer::Clear(GBufferRenderTextures_[i]);
  }
  Renderer::Clear(GBufferDepthTexture_);

  //  GBufferPass
  {
    Renderer::SetRenderTextures(GBufferRenderTextures_, GBufferDepthTexture_);
    {
      Renderer3D::Draw(Rectangle_, Matrix::Scale(kRectangleScale) * Matrix::Rotate(Math::ToRadian(90.0f), 0.0f, 0.0f));
      for (auto& Data : Datas_)
      {
        Renderer3D::Draw(Sphere_, Matrix::Translate(Data.Position), Data.Color);
      }
    }
  }

  //  LightPass
  {
    Renderer::SetRenderTexture(RenderTexture(), DepthStencilTexture());
    Renderer::SetPixelStageShaderResources(0, static_cast<vdl::uint>(PixelStageShaderResources_.size()), PixelStageShaderResources_.data());

    RenderingData& Data = RenderingConstantBuffer_.GetData();
    {
      Data.InverseViewProjection = (Renderer3D::GetView() * Renderer3D::GetProjection()).Inverse();
    }

    Renderer::Draw(4);
  }

  //  GBuffer‚Ì•`‰æ
  {
    decltype(PixelStageShaderResources_) ShaderResources;
    Renderer2D::SetPixelStageShaderResources(2, static_cast<vdl::uint>(ShaderResources.size()) - 2, ShaderResources.data());

    for (vdl::uint i = 0; i < kUseRenderTextureNum; ++i)
    {
      Renderer2D::Draw(GBufferRenderTextures_[i], float2(kGBufferLeftPos, kGBufferDisplaySize.y * i), kGBufferDisplaySize);
    }
    Renderer2D::Draw(GBufferDepthTexture_, float2(kGBufferLeftPos, kGBufferDisplaySize.y * kUseRenderTextureNum), kGBufferDisplaySize);
  }
}
