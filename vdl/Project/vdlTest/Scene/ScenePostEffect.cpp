#include "ScenePostEffect.hpp"

using namespace vdl;

void ScenePostEffect::Initialize()
{
  House_ = Model("Data/house_obj/house_obj.obj");
  Ground_ = StaticModelData::Rectangle("Data/GRASSX1/GRASSX1.jpg", "Data/GRASSX1/GRASSX1_normal.jpg");

  Camera_ = Camera(float3(0.0f, 10.0f, -25.0f));

  HouseWorld_ = Matrix::Identity();
  GroundWorld_ = Matrix::Scale(kGroundScale) * Matrix::Rotate(Math::ToRadian(90.0f), 0.0f, 0.0f);

  //  GBufferPassの初期化
  {
    GBufferPassVertexShader_ = VertexShader("Shader/GBufferPass/GBufferPassVS.hlsl", InputLayoutType::eStaticMesh);
    GBufferPassPixelShader_ = PixelShader("Shader/PostEffect/GBufferPassPS.hlsl");

    //  DiffuseMap
    GBufferRenderTextures_[0] = RenderTexture(kGBufferSize, FormatType::eR8G8B8A8_Unorm);
    //  NormalMap
    GBufferRenderTextures_[1] = RenderTexture(kGBufferSize, FormatType::eR16G16B16A16_Float);

    GBufferDepthTexture_ = DepthStencilTexture(kGBufferSize, FormatType::eD32_Float);
  }

  //  ShadowMapの初期化
  {
    ShadowMapVertexShader_ = VertexShader("Shader/PostEffect/ShadowMapVS.hlsl", InputLayoutType::eStaticMesh);
    ShadowMapPixelShader_ = PixelShader();
    ShadowMap_ = DepthStencilTexture(kShadowMapSize, FormatType::eD32_Float);

    DirectionLightPosition_ = Camera_.Position;

    Renderer3D::SetVertexStageConstantBuffers(1, 1, &LightViewProjectionConstantBuffer_);
  }

  //  LightPassの初期化
  {
    LightPassVertexShader_ = VertexShader("Shader/Option/FullScreenTriangleVS.hlsl", InputLayoutType::eNone);
    LightPassPixelShader_ = PixelShader(kLigthPassPSFilePath);
    Renderer::SetShaders(LightPassVertexShader_, LightPassPixelShader_);

    //  Color
    RenderTextures_[0] = RenderTexture(kWindowSize, FormatType::eR8G8B8A8_Unorm);
    //  Luminance
    RenderTextures_[1] = RenderTexture(kWindowSize, FormatType::eR16G16B16A16_Float);

    ShadowMapSampler_ = Sampler(AddressModeType::eBorder, AddressModeType::eBorder, AddressModeType::eBorder,
      FilterType::eAnisotropic, 16, BorderColorType::eWhite);
    Renderer::SetPixelStageSamplers(0, 1, &ShadowMapSampler_);

    for (vdl::uint i = 0; i < kGBufferNum; ++i)
    {
      ShaderResources_[i] = GBufferRenderTextures_[i];
    }
    ShaderResources_[kGBufferNum + 0] = GBufferDepthTexture_.GetDepthTexture();
    ShaderResources_[kGBufferNum + 1] = ShadowMap_.GetDepthTexture();

    DirectionalLightConstantBuffer_.GetData() = { float3(float3(0.0f) - DirectionLightPosition_).Normalize(), 1.5f, Palette::White };
    Renderer::SetPixelStageConstantBuffers(0, 1, &DirectionalLightConstantBuffer_);

    RenderingData& RenderingData = RenderingConstantBuffer_.GetData();
    {
      RenderingData.LuminanceThreshold = 0.45f;
      RenderingData.Shadow = ColorF(Palette::DimGray, 0.0001f);
      RenderingData.Fog = ColorF(Palette::DimGray, 5.0f);
    }
    Renderer::SetPixelStageConstantBuffers(1, 1, &RenderingConstantBuffer_);
    Renderer::SetPixelStageConstantBuffers(2, 1, &LightViewProjectionConstantBuffer_);
    Renderer::SetTopology(TopologyType::eTriangleStrip);
  }

  //  PostProcessの初期化
  {
    VerticalGaussianBlurPixelShader_ = PixelShader("Shader/PostEffect/GaussianBlurPS.hlsl", "VertexBlur");
    HorizontalGaussianBlurPixelShader_ = PixelShader("Shader/PostEffect/GaussianBlurPS.hlsl", "HorizontalBlur");
    BloomPixelShader_ = PixelShader("Shader/PostEffect/BloomPS.hlsl");
    TexturePixelShader_ = PixelShader("Shader/Texture/TexturePS.hlsl");

    vdl::uint2 TextureSize = kWindowSize;
    for (vdl::uint i = 0; i < kShrinkBuffeNum; ++i)
    {
      ShrinkBuffers_[i] = RenderTexture(TextureSize, FormatType::eR8G8B8A8_Unorm);
      ShrinkDepthBuffer_[i] = DepthStencilTexture(TextureSize, FormatType::eD16_Unorm);
      BloomShaderResources_[i] = ShrinkBuffers_[i];

      TextureSize /= 2;
    }
  }
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

    ImGui::Begin("ScenePostEffect");
    {
#if defined _DEBUG | DEBUG
      if (ImGui::Button("Reload LightPassPS"))
      {
        LightPassPixelShader_ = PixelShader(kLigthPassPSFilePath);
        Renderer::SetPixelShader(LightPassPixelShader_);
      }
#endif
      ImGui::ColorEdit3("Shadow", &RenderingData.Shadow);
      ImGui::InputFloat("ShadowBias", &RenderingData.Shadow.Alpha);
      ImGui::ColorEdit3("Fog", &RenderingData.Fog);
      ImGui::SliderFloat("LuminanceThreshold", &RenderingData.LuminanceThreshold, 0.0f, 1.0f);
      ImGui::InputFloat("Exposure", &RenderingData.Fog.Alpha);
      if (ImGui::TreeNode("DirectionalLight"))
      {
        if (ImGui::InputFloat3("Position", &DirectionLightPosition_.x))
        {
          DirectionalLight.Direction = float3(float3(0.0f) - DirectionLightPosition_).Normalize();
        }
        ImGui::Text(std::string("Direction:" + std::to_string(DirectionalLight.Direction)).c_str());
        ImGui::InputFloat("Itensity", &DirectionalLight.Itensity);
        ImGui::ColorEdit3("Color", &DirectionalLight.Color);
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("ShaderResources"))
      {
        for (vdl::uint i = 0; i < kShaderResourceNum; ++i)
        {
          if (ImGui::CollapsingHeader(kShaderResourceNames[i]))
          {
            ImGui::Image(ShaderResources_[i], kGBufferDisplaySize);
          }
        }
        if (ImGui::CollapsingHeader(kShaderResourceNames[kShaderResourceNum + 0]))
        {
          for (vdl::uint ShrinkBufferCount = 0; ShrinkBufferCount < kShrinkBuffeNum; ++ShrinkBufferCount)
          {
            ImGui::Text(std::string("TextureSize:" + std::to_string(ShrinkBuffers_[ShrinkBufferCount].GetSize())).c_str());
            ImGui::Image(ShrinkBuffers_[ShrinkBufferCount], kGBufferDisplaySize);
          }
        }

        ImGui::TreePop();
      }

    }
    ImGui::End();

    const Camera Light = Camera(DirectionLightPosition_, float3(0.0f), float3::Up());
    LightViewProjectionConstantBuffer_.GetData() = Light.View() * Light.Projection(kWindowSize);
  }

  //  画面のクリア
  {
    for (uint i = 0; i < kGBufferNum; ++i)
    {
      Renderer::Clear(GBufferRenderTextures_[i]);
    }
    Renderer::Clear(GBufferDepthTexture_);
    Renderer::Clear(ShadowMap_);
    for (uint i = 0; i < kShrinkBuffeNum; ++i)
    {
      Renderer::Clear(ShrinkBuffers_[i]);
      Renderer::Clear(ShrinkDepthBuffer_[i]);
    }
  }

  //  GBufferPass
  {
    Renderer::SetRenderTextures(GBufferRenderTextures_, GBufferDepthTexture_);
    Renderer3D::SetStaticMeshShaders(GBufferPassVertexShader_, GBufferPassPixelShader_);

    Renderer3D::Draw(House_, HouseWorld_);
    Renderer3D::Draw(Ground_, GroundWorld_);
  }

  //  ShadowPass
  {
    Renderer::SetRenderTexture(RenderTexture(), ShadowMap_);
    Renderer3D::SetStaticMeshShaders(ShadowMapVertexShader_, ShadowMapPixelShader_);

    Renderer3D::Draw(House_, HouseWorld_);
  }

  //  LightPass
  {
    Renderer::SetRenderTextures(RenderTextures_, DepthStencilTexture());
    Renderer::SetPixelStageShaderResources(0, static_cast<uint>(ShaderResources_.size()), ShaderResources_.data());

    Renderer::Draw(3);
  }

  //  PostProcess
  {
    decltype(BloomShaderResources_) ShaderResources;
    Renderer2D::SetPixelStageShaderResources(1, static_cast<uint>(ShaderResources.size()), ShaderResources.data());

    //  Blur
    {
      Texture SrcTexture = RenderTextures_[1];
      Viewport Viewport = { 0, 0 };

      for (uint i = 0; i < kShrinkBuffeNum; ++i)
      {
        Renderer::SetRenderTexture(ShrinkBuffers_[i], ShrinkDepthBuffer_[i]);

        Viewport.Size = ShrinkBuffers_[i].GetSize();
        Renderer2D::SetViewport(Viewport);

        Renderer2D::SetPixelShader(VerticalGaussianBlurPixelShader_);
        Renderer2D::Draw(SrcTexture, 0.0f, Viewport.Size);

        Renderer2D::SetPixelShader(HorizontalGaussianBlurPixelShader_);
        Renderer2D::Draw(SrcTexture, 0.0f, Viewport.Size);

        SrcTexture = ShrinkBuffers_[i];
      }

      Viewport.Size = kWindowSize;
      Renderer2D::SetViewport(Viewport);
    }

    //  Bloom
    {
      Renderer::SetRenderTexture(Window::GetRenderTexture(), Window::GetDepthStencilTexture());
      Renderer2D::SetPixelShader(BloomPixelShader_);
      Renderer2D::SetPixelStageShaderResources(1, kShrinkBuffeNum, BloomShaderResources_.data());

      Renderer2D::Draw(RenderTextures_[0]);
    }
  }
}
