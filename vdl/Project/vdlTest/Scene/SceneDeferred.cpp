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
  Rectangle_ = MeshData::Rectangle("Data/asphalt/asphalt.jpg", "Data/asphalt/asphalt_normal.jpg");
  RectangleSpecularMap_ = Texture("Data/asphalt/asphalt_specular.jpg");
  Sphere_ = MeshData::Sphere(12, 12, "Data/earthmap/earthmap.jpg", "Data/earthmap/earthnormal.jpg");
  SphereSpecularMap_ = Texture("Data/earthmap/earthspec.jpg");

  Camera_ = Camera(float3(0.0f, 5.0f, -25.0f));
  PointLightItensity_ = 0.5f;
  PointLightRange_ = 0.75f;

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

  //  GBufferPassの初期化
  {
    GBufferPassVertexShader_ = VertexShader("Shader/Deferred/GBufferPassVS.hlsl", InputLayoutType::eMesh);
    GBufferPassPixelShader_ = PixelShader("Shader/Deferred/GBufferPassPS.hlsl");

    GBufferRenderTextures_[0] = RenderTexture(kWindowSize, FormatType::eR8G8B8A8_Unorm);
    GBufferRenderTextures_[1] = RenderTexture(kWindowSize, FormatType::eR16G16B16A16_Float);
    GBufferRenderTextures_[2] = RenderTexture(kWindowSize, FormatType::eR8G8B8A8_Unorm);
    GBufferDepthTexture_ = DepthStencilTexture(kWindowSize, FormatType::eD32_Float);
  }

  //  ShadowMapの初期化
  {
    ShadowMapVertexShader_ = VertexShader("Shader/Deferred/ShadowMapVS.hlsl", InputLayoutType::eMesh);
    ShadowMapPixelShader_ = PixelShader();
    ShadowMap_ = DepthStencilTexture(kWindowSize, FormatType::eD32_Float);

    DirectionLightPosition_ = Camera_.Position;

    Renderer3D::SetVertexStageConstantBuffers(1, 1, &LightViewProjectionConstantBuffer_);
  }

  //  LightPassの初期化
  {
    LightPassVertexShader_ = VertexShader("Shader/Deferred/LightPassVS.hlsl", InputLayoutType::eNone);
    LightPassPixelShader_ = PixelShader(kLigthPassPSFilePath);
    ShadowMapSampler_ = Sampler(AddressModeType::eBorder, AddressModeType::eBorder, AddressModeType::eBorder,
      FilterType::eAnisotropic, 16, BorderColorType::eWhite);
    Renderer::SetPixelStageSamplers(0, 1, &ShadowMapSampler_);

    for (vdl::uint i = 0; i < kUseRenderTextureNum; ++i)
    {
      PixelStageShaderResources_[i] = GBufferRenderTextures_[i];
    }
    PixelStageShaderResources_[kUseRenderTextureNum] = GBufferDepthTexture_;
    PixelStageShaderResources_[kUseRenderTextureNum + 1] = ShadowMap_;

    Renderer::SetShaders(LightPassVertexShader_, LightPassPixelShader_);
    LightData& LightData = LightConstantBuffer_.GetData();
    {
      LightData.DirectionalLight.Direction = float3(float3(0.0f) - DirectionLightPosition_).Normalize();
      LightData.DirectionalLight.Itensity = 0.125f;
      LightData.DirectionalLight.Color = Palette::White;
    }
    Renderer::SetPixelStageConstantBuffers(0, 1, &LightConstantBuffer_);
    RenderingData& RenderingData = RenderingConstantBuffer_.GetData();
    {
      RenderingData.SpecularPower = 15.0f;
      RenderingData.Ambient = Palette::DimGray;
      RenderingData.Shadow = ColorF(Palette::DimGray, 0.0001f);
    }
    Renderer::SetPixelStageConstantBuffers(1, 1, &RenderingConstantBuffer_);
    Renderer::SetPixelStageConstantBuffers(2, 1, &LightViewProjectionConstantBuffer_);
    Renderer::SetTopology(TopologyType::eTriangleStrip);
  }
}

SceneDeferred::~SceneDeferred()
{
  Renderer3D::SetShaders(VertexShader("Shader/Mesh/MeshVS.hlsl", InputLayoutType::eMesh),
    PixelShader("Shader/Mesh/MeshPS.hlsl"));
  Renderer::SetTopology(TopologyType::eDefaultNone);
}

void SceneDeferred::Update()
{
  //  情報の更新
  {
    FreeCamera(&Camera_);
    Renderer3D::SetCamera(Camera_);

    LightData& LightData = LightConstantBuffer_.GetData();

    RenderingData& RenderingData = RenderingConstantBuffer_.GetData();
    {
      RenderingData.EyePosition = Camera_.Position;
      RenderingData.InverseViewProjection = (Renderer3D::GetView() * Renderer3D::GetProjection()).Inverse();
    }

    ImGui::Begin("SceneDeferred");
    {
      if (ImGui::Button("Reload LightPassPS"))
      {
        LightPassPixelShader_ = PixelShader(kLigthPassPSFilePath);
        Renderer::SetPixelShader(LightPassPixelShader_);
      }
      ImGui::Checkbox("SphereUpdate", &isUpdate_);
      ImGui::InputFloat("SpecularPower", &RenderingData.SpecularPower);
      ImGui::ColorEdit3("Ambient", &RenderingData.Ambient.Red);
      ImGui::ColorEdit3("Shadow", &RenderingData.Shadow.Red);
      ImGui::InputFloat("ShadowBias", &RenderingData.Shadow.Alpha);
      if (ImGui::TreeNode("DirectionalLight"))
      {
        if (ImGui::InputFloat3("Position", &DirectionLightPosition_.x))
        {
          LightData.DirectionalLight.Direction = float3(float3(0.0f) - DirectionLightPosition_).Normalize();
        }
        ImGui::Text(std::string("Direction:" + std::to_string(LightData.DirectionalLight.Direction)).c_str());
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

    const Camera Light = Camera(DirectionLightPosition_, float3(0.0f), float3::Up()/*, 0.1f, 300.0f*/);
    LightViewProjectionConstantBuffer_.GetData() = Light.View() * Light.Projection(kWindowSize);

    if (isUpdate_)
    {
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
  }

  //  画面のクリア
  {
    for (vdl::uint i = 0; i < kUseRenderTextureNum; ++i)
    {
      Renderer::Clear(GBufferRenderTextures_[i]);
    }
    Renderer::Clear(GBufferDepthTexture_);
    Renderer::Clear(ShadowMap_);
  }

  const Matrix RectangleWorld = Matrix::Scale(kRectangleScale) * Matrix::Rotate(Math::ToRadian(90.0f), 0.0f, 0.0f);
  const Matrix SphereScale = Matrix::Scale(kSphereScale);
  //  GBufferPass
  {
    Renderer::SetRenderTextures(GBufferRenderTextures_, GBufferDepthTexture_);
    Renderer3D::SetShaders(GBufferPassVertexShader_, GBufferPassPixelShader_);

    Renderer3D::SetPixelStageShaderResources(2, 1, &RectangleSpecularMap_);
    Renderer3D::Draw(Rectangle_, RectangleWorld);

    Renderer3D::SetPixelStageShaderResources(2, 1, &SphereSpecularMap_);
    for (auto& Data : Datas_)
    {
      Renderer3D::Draw(Sphere_, SphereScale * Matrix::Translate(Data.Position), Data.Color);
    }
  }

  //  ShadowPass
  {
    Renderer::SetRenderTexture(RenderTexture(), ShadowMap_);
    Renderer3D::SetShaders(ShadowMapVertexShader_, ShadowMapPixelShader_);

    for (auto& Data : Datas_)
    {
      Renderer3D::Draw(Sphere_, SphereScale * Matrix::Translate(Data.Position), Data.Color);
    }
  }

  //  LightPass
  {
    Renderer::SetRenderTexture(RenderTexture(), DepthStencilTexture());
    Renderer::SetPixelStageShaderResources(0, static_cast<vdl::uint>(PixelStageShaderResources_.size()), PixelStageShaderResources_.data());

    Renderer::Draw(4);
  }

  //  GBufferの描画
  {
    decltype(PixelStageShaderResources_) ShaderResources;
    Renderer2D::SetPixelStageShaderResources(2, static_cast<vdl::uint>(ShaderResources.size()) - 2, ShaderResources.data());

    for (vdl::uint i = 0; i < PixelStageShaderResources_.size(); ++i)
    {
      Renderer2D::Draw(PixelStageShaderResources_[i], float2(kGBufferLeftPos, kGBufferDisplaySize.y * i), kGBufferDisplaySize);
    }
  }
}
