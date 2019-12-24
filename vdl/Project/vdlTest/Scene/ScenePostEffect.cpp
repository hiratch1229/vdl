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
    LightPassRenderTextures_[0] = RenderTexture(kWindowSize, FormatType::eR8G8B8A8_Unorm);
    //  Luminance
    LightPassRenderTextures_[1] = RenderTexture(kWindowSize, FormatType::eR16G16B16A16_Float);

    for (uint i = 0; i < kGBufferNum; ++i)
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
      RenderingData.Fog = Palette::DimGray;
      RenderingData.Exposure = 5.0f;
      RenderingData.Shadow = Palette::DimGray;
      RenderingData.ShadowBias = 0.0001f;
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

    uint2 TextureSize = kWindowSize;
    for (uint i = 0; i < kShrinkBuffeNum; ++i)
    {
      ShrinkBuffers_[i] = RenderTexture(TextureSize, FormatType::eR8G8B8A8_Unorm);

      TextureSize /= 2;
    }

    Sampler ClampSampler = Sampler::kDefault3D;
    ClampSampler.AddressModeU = ClampSampler.AddressModeV = ClampSampler.AddressModeW = AddressModeType::eClamp;
    Renderer2D::SetPixelStageSamplers(1, 1, &ClampSampler);
  }
}

void ScenePostEffect::Update()
{
  //  情報の更新
  {
    FreeCamera(&Camera_);
    Renderer3D::SetCamera(Camera_);

    DirectinalLight& DirectionalLight = DirectionalLightConstantBuffer_.GetData();

    RenderingData& RenderingData = RenderingConstantBuffer_.GetData();
    {
      RenderingData.EyePos = Camera_.Position;
      RenderingData.InverseViewProjection = (Camera_.View() * Camera_.Projection(kWindowSize)).Inverse();
    }

    constexpr GUI::WindowFlags kSceneWindowBeginFlags = GUI::WindowFlags(GUI::WindowFlag::eNoMove) | GUI::WindowFlag::eNoResize;
    GUI::Begin("ScenePostEffect", nullptr, kSceneWindowBeginFlags);
    {
      GUI::SetWindowPos(GUIHelper::kSceneWindowPos);
      GUI::SetWindowSize(kSceneWindowSize);
#if defined _DEBUG | DEBUG
      if (GUI::Button("Reload LightPassPS"))
      {
        LightPassPixelShader_ = PixelShader(kLigthPassPSFilePath);
        Renderer::SetPixelShader(LightPassPixelShader_);
      }
#endif
      GUI::ColorEdit("Shadow", &RenderingData.Shadow);
      GUI::Input("ShadowBias", &RenderingData.ShadowBias, 0.0f, 0.0f, "%.6f");
      GUI::ColorEdit("Fog", &RenderingData.Fog);
      GUI::Slider("LuminanceThreshold", &RenderingData.LuminanceThreshold, 0.0f, 1.0f);
      GUI::Drag("Exposure", &RenderingData.Exposure, 0.01f);
      if (GUI::TreeNode("DirectionalLight"))
      {
        if (GUI::Drag("Position", &DirectionLightPosition_))
        {
          DirectionalLight.Direction = float3(float3(0.0f) - DirectionLightPosition_).Normalize();
        }
        GUI::Text(std::string("Direction:" + std::to_string(DirectionalLight.Direction)).c_str());
        GUI::Drag("Itensity", &DirectionalLight.Itensity, 0.01f);
        GUI::ColorEdit("Color", &DirectionalLight.Color);
        GUI::TreePop();
      }
    }
    GUI::End();
  }

  const Camera Light = Camera(DirectionLightPosition_, float3(0.0f), float3::Up(), 5.0f, 500.0f, 30.0f, false);
  LightViewProjectionConstantBuffer_.GetData() = Light.View() * Light.Projection(kWindowSize / 4.0f);

  //  画面のクリア
  {
    for (uint i = 0; i < kGBufferNum; ++i)
    {
      Renderer::Clear(GBufferRenderTextures_[i]);
    }
    Renderer::Clear(GBufferDepthTexture_);
    Renderer::Clear(ShadowMap_);
    for (uint i = 0; i < kLightPassRenderTextureNum; ++i)
    {
      Renderer::Clear(LightPassRenderTextures_[i]);
    }
    for (uint i = 0; i < kShrinkBuffeNum; ++i)
    {
      Renderer::Clear(ShrinkBuffers_[i]);
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

    Viewport Viewport = { float2(0.0f), kShadowMapSize };
    Renderer3D::SetViewport(Viewport);

    Renderer3D::SetStaticMeshShaders(ShadowMapVertexShader_, ShadowMapPixelShader_);

    Renderer3D::Draw(House_, HouseWorld_);

    Viewport.Size = Constants::kDefaultWindowSize;
    Renderer3D::SetViewport(Viewport);
  }

  //  LightPass
  {
    Renderer::SetRenderTextures(LightPassRenderTextures_, DepthStencilTexture());
    Renderer::SetPixelStageShaderResources(0, static_cast<uint>(ShaderResources_.size()), ShaderResources_.data());

    Renderer::Draw(3);
  }

  //  PostProcess
  {
    std::array<vdl::ShaderResource, kShrinkBuffeNum> ShaderResources;
    Renderer2D::SetPixelStageShaderResources(1, static_cast<uint>(ShaderResources.size()), ShaderResources.data());

    //  Blur
    {
      Texture SrcTexture = LightPassRenderTextures_[1];
      Viewport Viewport = { 0, 0 };

      for (uint i = 0; i < kShrinkBuffeNum; ++i)
      {
        Renderer::SetRenderTexture(ShrinkBuffers_[i], DepthStencilTexture());

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
      Renderer2D::SetPixelStageShaderResources(1, kShrinkBuffeNum, ShrinkBuffers_.data());

      Renderer2D::Draw(LightPassRenderTextures_[0]);
    }
  }

  constexpr GUI::WindowFlags kRenderingFlowBeginFlags = GUI::WindowFlags(GUI::WindowFlag::eNoMove) | GUI::WindowFlag::eNoResize;
  GUI::Begin("RenderingFlow", nullptr, kRenderingFlowBeginFlags);
  {
    GUI::SetWindowPos(GUIHelper::kRenderingFlowWindowPos);
    GUI::SetWindowSize(GUIHelper::kRenderingFlowWindowSize);

    if (GUI::TreeNode("GBufferPass"))
    {
      GUIHelper::DrawRenderTexture("Diffuse", GBufferRenderTextures_[0], GUIHelper::kGBufferDisplaySize);
      GUIHelper::DrawRenderTexture("Normal", GBufferRenderTextures_[1], GUIHelper::kGBufferDisplaySize);
      GUIHelper::DrawDepthTexture("Depth", GBufferDepthTexture_, GUIHelper::kGBufferDisplaySize);

      GUI::TreePop();
    }
    if (GUI::TreeNode("ShadowPass"))
    {
      GUIHelper::DrawDepthTexture("ShadowMap", ShadowMap_, kShadowMapDisplaySize);

      GUI::TreePop();
    }
    if (GUI::TreeNode("LightPass"))
    {
      GUIHelper::DrawRenderTexture("Color", LightPassRenderTextures_[0], GUIHelper::kGBufferDisplaySize);
      GUIHelper::DrawRenderTexture("Luminance", LightPassRenderTextures_[1], GUIHelper::kGBufferDisplaySize);

      GUI::TreePop();
    }
    if (GUI::TreeNode("PostProcess"))
    {
      for (vdl::uint ShrinkBufferCount = 0; ShrinkBufferCount < kShrinkBuffeNum; ++ShrinkBufferCount)
      {
        GUIHelper::DrawRenderTexture(std::string("ShrinkBuffer" + std::to_string(ShrinkBufferCount)).c_str(), ShrinkBuffers_[ShrinkBufferCount], GUIHelper::kGBufferDisplaySize);
      }

      GUI::TreePop();
    }
  }
  GUI::End();
}
