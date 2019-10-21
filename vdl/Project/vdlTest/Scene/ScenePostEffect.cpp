#include "ScenePostEffect.hpp"

using namespace vdl;

void ScenePostEffect::Initialize()
{
  House_ = Model("Data/house_obj/house_obj.obj");
  Ground_ = MeshData::Rectangle("Data/GRASSX1/GRASSX1.jpg", "Data/GRASSX1/GRASSX1_normal.jpg");

  Camera_ = Camera(float3(0.0f, 10.0f, -25.0f));

  HouseWorld_ = Matrix::Identity();
  GroundWorld_ = Matrix::Scale(kGroundScale) * Matrix::Rotate(Math::ToRadian(90.0f), 0.0f, 0.0f);

  //  GBufferPassの初期化
  {
    GBufferPassVertexShader_ = VertexShader("Shader/PostEffect/GBufferPassVS.hlsl", InputLayoutType::eMesh);
    GBufferPassPixelShader_ = PixelShader("Shader/PostEffect/GBufferPassPS.hlsl");

    GBufferRenderTextures_[0] = RenderTexture(kGBufferSize, FormatType::eR8G8B8A8_Unorm);
    GBufferRenderTextures_[1] = RenderTexture(kGBufferSize, FormatType::eR16G16B16A16_Float);
    GBufferDepthTexture_ = DepthStencilTexture(kGBufferSize, FormatType::eD16_Unorm);
  }

  //  ShadowMapの初期化
  {
    ShadowMapVertexShader_ = VertexShader("Shader/PostEffect/ShadowMapVS.hlsl", InputLayoutType::eMesh);
    ShadowMapPixelShader_ = PixelShader();
    ShadowMap_ = DepthStencilTexture(kShadowMapSize, FormatType::eD32_Float);

    DirectionLightPosition_ = Camera_.Position;

    Renderer3D::SetVertexStageConstantBuffers(1, 1, &LightViewProjectionConstantBuffer_);
  }

  //  LightPassの初期化
  {
    LightPassVertexShader_ = VertexShader("Shader/Deferred/LightPassVS.hlsl", InputLayoutType::eNone);
    LightPassPixelShader_ = PixelShader(kLigthPassPSFilePath);
    Renderer::SetShaders(LightPassVertexShader_, LightPassPixelShader_);

    ShadowMapSampler_ = Sampler(AddressModeType::eBorder, AddressModeType::eBorder, AddressModeType::eBorder,
      FilterType::eAnisotropic, 16, BorderColorType::eWhite);
    Renderer::SetPixelStageSamplers(0, 1, &ShadowMapSampler_);

    for (vdl::uint i = 0; i < kUseRenderTextureNum; ++i)
    {
      PixelStageShaderResources_[i] = GBufferRenderTextures_[i];
    }
    PixelStageShaderResources_[kUseRenderTextureNum] = GBufferDepthTexture_;
    PixelStageShaderResources_[kUseRenderTextureNum + 1] = ShadowMap_;

    DirectionalLightConstantBuffer_.GetData() = { float3(float3(0.0f) - DirectionLightPosition_).Normalize(), 0.125f, Palette::White };
    Renderer::SetPixelStageConstantBuffers(0, 1, &DirectionalLightConstantBuffer_);

    RenderingConstantBuffer_.GetData().Shadow = ColorF(Palette::DimGray, 0.0001f);
    Renderer::SetPixelStageConstantBuffers(1, 1, &RenderingConstantBuffer_);
    Renderer::SetPixelStageConstantBuffers(2, 1, &LightViewProjectionConstantBuffer_);
    Renderer::SetTopology(TopologyType::eTriangleStrip);
  }
}

ScenePostEffect::~ScenePostEffect()
{
  Renderer3D::SetShaders(VertexShader("Shader/Mesh/MeshVS.hlsl", InputLayoutType::eMesh),
    PixelShader("Shader/Mesh/MeshPS.hlsl"));
  Renderer::SetTopology(TopologyType::eDefaultNone);
}

void ScenePostEffect::Update()
{
  //  情報の更新
  {
    FreeCamera(&Camera_);
    Renderer3D::SetCamera(Camera_);

    vdl::DirectinalLight& DirectionalLight = DirectionalLightConstantBuffer_.GetData();

    RenderingData& RenderingData = RenderingConstantBuffer_.GetData();
    {
      RenderingData.EyePos = Camera_.Position;
      RenderingData.InverseViewProjection = (Renderer3D::GetView() * Renderer3D::GetProjection()).Inverse();
    }

    ImGui::Begin("SceneDeferred");
    {
      if (ImGui::Button("Reload LightPassPS"))
      {
        LightPassPixelShader_ = PixelShader(kLigthPassPSFilePath);
        Renderer::SetPixelShader(LightPassPixelShader_);
      }
      ImGui::ColorEdit3("Shadow", &RenderingData.Shadow.Red);
      ImGui::InputFloat("ShadowBias", &RenderingData.Shadow.Alpha);
      if (ImGui::TreeNode("DirectionalLight"))
      {
        if (ImGui::InputFloat3("Position", &DirectionLightPosition_.x))
        {
          DirectionalLight.Direction = float3(float3(0.0f) - DirectionLightPosition_).Normalize();
        }
        ImGui::Text(std::string("Direction:" + std::to_string(DirectionalLight.Direction)).c_str());
        ImGui::InputFloat("Itensity", &DirectionalLight.Itensity);
        ImGui::ColorEdit3("Color", &DirectionalLight.Color.Red);
        ImGui::TreePop();
      }
    }
    ImGui::End();

    const Camera Light = Camera(DirectionLightPosition_, float3(0.0f), float3::Up()/*, 0.1f, 300.0f*/);
    LightViewProjectionConstantBuffer_.GetData() = Light.View() * Light.Projection(kWindowSize);
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

  //  GBufferPass
  {
    Renderer::SetRenderTextures(GBufferRenderTextures_, GBufferDepthTexture_);
    Renderer3D::SetShaders(GBufferPassVertexShader_, GBufferPassPixelShader_);

    Renderer3D::Draw(House_, HouseWorld_);
    Renderer3D::Draw(Ground_, GroundWorld_);
  }

  //  ShadowPass
  {
    Renderer::SetRenderTexture(RenderTexture(), ShadowMap_);
    Renderer3D::SetShaders(ShadowMapVertexShader_, ShadowMapPixelShader_);

    Renderer3D::Draw(House_, HouseWorld_);
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
