#include "SceneOcean.hpp"

using namespace vdl;

void SceneOcean::Initialize()
{
  StaticModelData ModelData;
  {
    ModelData.Vertices.resize(4);
    {
      ModelData.Vertices[0] = { { +kRetangleHalfSize.x, 0.0f, +kRetangleHalfSize.y }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
      ModelData.Vertices[1] = { { -kRetangleHalfSize.x, 0.0f, +kRetangleHalfSize.y }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
      ModelData.Vertices[2] = { { +kRetangleHalfSize.x, 0.0f, -kRetangleHalfSize.y }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };
      ModelData.Vertices[3] = { { -kRetangleHalfSize.x, 0.0f, -kRetangleHalfSize.y }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
    }
    ModelData.Indices = { 0, 1, 2, 3 };
    ModelData.MeshDatas.resize(1);
    {
      ModelData.MeshDatas[0].IndexCount = 4;
      ModelData.MeshDatas[0].Material.Diffuse = Palette::Diffuse;
    }
  }
  Rectangle_ = ModelData;
  Sphere_ = StaticModelData::Sphere(20, 20);

  Camera_ = Camera(float3(0.0f, 15.0f, -60.0f), float3(0.0f, 0.0f, 0.0f), float3::Up(), kCameraNear, kCameraFar);
  SwapchainRenderTexture_ = Window::GetRenderTexture();
  DepthTexture_ = DepthStencilTexture(kGBufferSize, FormatType::eD32_Float);
  RasterizerState_ = RasterizerState::kSolidCullNone;

  CameraData& CameraData = CameraDataConstantBuffer_.GetData();
  {
    CameraData.EyePostion = Camera_.Position;
    CameraData.ViewProjection = Camera_.View() * Camera_.Projection(Constants::kDefaultWindowSize);
    CameraData.InverseViewProjection = CameraData.ViewProjection.Inverse();
    CameraData.ViewVector = Camera_.ViewVector().Normalize();
    CameraData.FocalLength = 0.5f;
  }
  LightData& LightData = LightDataConstantBuffer_.GetData();
  {
    LightData.DirectionalLight.Direction = -DirectionalLightPosition_.Normalize();
    LightData.DirectionalLight.Color = Palette::White;
    LightData.DirectionalLight.Itensity = 3.0f;
  }

  //  ShadowPassの初期化
  {
    ShadowMap_ = DepthStencilTexture(kShadowMapSize, FormatType::eD32_Float);
    DirectionalLightPosition_ = float3(0.0f, 150.0f, -350.0f);
    UpdateLightViewProjecion();

    ShadowData& ShadowData = ShadowDataConstantBuffer_.GetData();
    {
      ShadowData.Shadow = ColorF(Palette::DimGray, 0.0001f);
    }
  }

  //  GBufferPassの初期化
  {
    GBufferTextures_[static_cast<uint>(GBufferType::eDiffuse)] = RenderTexture(kGBufferSize, FormatType::eR8G8B8A8_Unorm);
    GBufferTextures_[static_cast<uint>(GBufferType::eNormal)] = RenderTexture(kGBufferSize, FormatType::eR8G8B8A8_Unorm);
    GBufferTextures_[static_cast<uint>(GBufferType::eDepth)] = RenderTexture(kGBufferSize, FormatType::eR32_Float);
    StaticMeshVertexShader_ = VertexShader("Shader/Ocean/StaticMesh/StaticMeshVS.hlsl", InputLayoutType::eStaticMesh);
    StaticMeshPixelShader_ = PixelShader("Shader/Ocean/StaticMesh/StaticMeshPS.hlsl");
    FullScreenVertexShader_ = VertexShader("Shader/Option/FullScreenTriangleVS.hlsl", InputLayoutType::eNone);
  }

  //  PostProcessの初期化
  {
    LightPassRenderTextures_[static_cast<uint>(LightPassOutputType::eColor)] = RenderTexture(Constants::kDefaultWindowSize, FormatType::eR8G8B8A8_Unorm);
    //LightPassRenderTextures_[static_cast<uint>(LightPassOutputType::eLuminance)] = RenderTexture(Constants::kDefaultWindowSize, FormatType::eR8G8B8A8_Unorm);

    uint2 TextureSize = Constants::kDefaultWindowSize;
    for (uint i = 0; i < kShrinkBuffeNum; ++i)
    {
      ShrinkBuffers_[i] = RenderTexture(TextureSize, FormatType::eR8G8B8A8_Unorm);
      TextureSize /= 2;
    }

    VerticalGaussianBlurPixelShader_ = PixelShader("Shader/PostEffect/GaussianBlurPS.hlsl", "VertexBlur");
    HorizontalGaussianBlurPixelShader_ = PixelShader("Shader/PostEffect/GaussianBlurPS.hlsl", "HorizontalBlur");
    DepthOfFieldPixelShader_ = PixelShader("Shader/Ocean/PostProcess/DepthOfFieldPS.hlsl");
  }

  ////  PostProcessの初期化
  //{
  //  OutputRenderTextures_[0] = SwapchainRenderTexture_;
  //  OutputRenderTextures_[1] = RenderTexture(Constants::kDefaultWindowSize, FormatType::eR8G8B8A8_Unorm);
  //
  //  uint2 TextureSize = Constants::kDefaultWindowSize;
  //  for (uint i = 0; i < kShrinkBuffeNum; ++i)
  //  {
  //    ShrinkBuffers_[i] = RenderTexture(TextureSize, FormatType::eR8G8B8A8_Unorm);
  //    TextureSize /= 2;
  //  }
  //
  //  VerticalGaussianBlurPixelShader_ = PixelShader("Shader/PostEffect/GaussianBlurPS.hlsl", "VertexBlur");
  //  HorizontalGaussianBlurPixelShader_ = PixelShader("Shader/PostEffect/GaussianBlurPS.hlsl", "HorizontalBlur");
  //  BloomPixelShader_ = PixelShader("Shader/PostEffect/BloomPS.hlsl");
  //}

  //  空の初期化
  {
    SkyboxTexture_ = CubeTexture("Data/skybox.png");
    SkyboxVertexShader_ = VertexShader("Shader/Skybox/SkyboxVS.hlsl", InputLayoutType::eStaticMesh);
    SkyboxPixelShader_ = PixelShader("Shader/Skybox/SkyboxPS.hlsl");
  }

  //  地面の初期化
  {
    TerrainTexcoordMap_ = RenderTexture(kGBufferSize, FormatType::eR16G16_Float);
    Renderer::Clear(TerrainTexcoordMap_, kTerrainTexcoordMapClearColor);
    TerrainHeightMap_ = UnorderedAccessTexture(kHeightMapSize, FormatType::eR32_Float);
    TerrainNormalMap_ = UnorderedAccessTexture(kHeightMapSize, FormatType::eR8G8B8A8_Unorm);
    Renderer::Clear(TerrainNormalMap_, Palette::NormalMap);
    TerrainTexcoordDepthTexture_ = DepthStencilTexture(kGBufferSize, FormatType::eD32_Float);
    TerrainHeightMapUpdateComputeShader_ = ComputeShader(kTerrainHeightMapUpdateComputeShaderFilePath);
    TerrainNormalMapUpdateComputeShader_ = ComputeShader(kTerrainNormalMapUpdateComputeShaderFilePath);
    TerrainUpdateData& TerrainUpdateData = TerrainUpdateDataConstantBuffer_.GetData();
    {
      TerrainUpdateData.BlushSize = 150.0f;
      TerrainUpdateData.BlushHardness = 1.0f;
    }

    for (uint i = 0; i < kTerrainTextureNum; ++i)
    {
      TerrainTextures_[i] = Texture(std::string(std::string("Data/Ocean/Terrain/") + kTerrainTextureFileNames[i]).c_str());
    }
    TerrainVertexShader_ = VertexShader("Shader/Ocean/Terrain/TerrainVS.hlsl", InputLayoutType::eStaticMesh);
    TerrainHullShader_ = HullShader("Shader/Ocean/Terrain/TerrainHS.hlsl");
    TerrainDomainShader_ = DomainShader("Shader/Ocean/Terrain/TerrainDS.hlsl", "Color");
    TerrainPixelShader_ = PixelShader(kTerrainPixelShaderFilePath, kTerrainPixelShaderEntryPoint);
    TerrainTexcoordMapDomainShader_ = DomainShader("Shader/Ocean/Terrain/TerrainDS.hlsl", "Texcoord");
    TerrainShadowDomainShader_ = DomainShader("Shader/Ocean/Terrain/TerrainDS.hlsl", "Shadow");
    TerrainTexcoordMapPixelShader_ = PixelShader("Shader/Ocean/Terrain/TerrainPS.hlsl", "HeightMapUpdatePrePass");
    TerrainData& TerrainData = TerrainDataConstantBuffer_.GetData();
    {
      TerrainData.TextureLoopNum = 5;
      TerrainData.SandThreshold = 5.0f;
      TerrainData.RockThreshold = 7.5f;
      TerrainData.SlopeThreshold = 10.0f;
    }
  }

  //  水面の初期化
  {
    WaterSurfaceGBufferTextures_[static_cast<uint>(GBufferType::eDiffuse)] = RenderTexture(kGBufferSize, FormatType::eR8G8B8A8_Unorm);
    WaterSurfaceGBufferTextures_[static_cast<uint>(GBufferType::eNormal)] = RenderTexture(kGBufferSize, FormatType::eR8G8B8A8_Unorm);
    WaterSurfaceGBufferTextures_[static_cast<uint>(GBufferType::eDepth)] = RenderTexture(kGBufferSize, FormatType::eR32_Float);
    char FIlePath[256];
    for (uint i = 0; i < kWaterSurfaceNormalMapNum; ++i)
    {
      sprintf_s(FIlePath, "Data/Ocean/WaterSurface/WaterNormalMap%d.png", i);
      WaterSurfaceNormalMaps_[i] = FIlePath;
    }

    WaterSurfaceVertexShader_ = VertexShader("Shader/Ocean/WaterSurface/WaterSurfaceVS.hlsl", InputLayoutType::eStaticMesh);
    WaterSurfaceHullShader_ = HullShader("Shader/Ocean/WaterSurface/WaterSurfaceHS.hlsl");
    WaterSurfaceDomainShader_ = DomainShader(kWaterSurfaceDomainShaderFilePath);
    WaterSurfacePixelShader_ = PixelShader(kWaterSurfacePixelShaderFilePath);

    std::array<Wave, kMaxWaveNum>& Waves = WaterSurfaceWaveDatasConstantBuffer_.GetData();
    {
      for (auto& Wave : Waves)
      {
        const float Radian = Random::Range(0.0f, Math::ToRadian(360.0f)) * 0.3f;
        Wave.Direction = float3(sinf(Radian), 0.0f, cosf(Radian));
        Wave.Length = Random::Range(kWaveMinLength, kWaveMaxLength);
        Wave.Speed = Random::Range(5.0f, 10.0f);
        //Wave.Speed = 1.0f;
        Wave.Steepness = Random::Range(0.15f, 1.0f);
        //Wave.Steepness = 0.8f;
        Wave.Amplitude = 0.03f + powf(2.0f, Random::Range(0.0f, 2.0f)) * Random::Range(0.0125f, 0.025f);
      }
    }
    WaterSurfaceData& WaterSurfaceData = WaterSurfaceDataConstantBuffer_.GetData();
    {
      WaterSurfaceData.Time = 0.0f;
      WaterSurfaceData.WaveNum = kMaxWaveNum;
    }
    RayMarchData& RayMarchData = RayMarchConstantBuffer_.GetData();
    {
      RayMarchData.MaxStep = 0.25f;
      RayMarchData.SampleNum = 50;
      RayMarchData.Step = RayMarchData.MaxStep / RayMarchData.SampleNum;
    }

    WaterSurfaceRefractionPixelShader_ = PixelShader(kWaterSurfaceRefractionPixelShaderFilePath);
    WaterSurfaceReflectionPixelShader_ = PixelShader(kWaterSurfaceReflectionPixelShaderFilePath);
  }

  //  Rendererのステートの設定
  {
    Renderer::SetTopology(TopologyType::eTriangleList);

    Renderer::SetVertexShader(FullScreenVertexShader_);

    Sampler ShadowSampler = Sampler::kDefault3D;
    ShadowSampler.AddressModeU = ShadowSampler.AddressModeV = ShadowSampler.AddressModeW = AddressModeType::eBorder;
    ShadowSampler.BorderColor = BorderColorType::eWhite;
    Renderer::SetPixelStageSamplers(1, 1, &ShadowSampler);
    Sampler ClampSampler = Sampler::kDefault3D;
    ShadowSampler.AddressModeU = ShadowSampler.AddressModeV = ShadowSampler.AddressModeW = AddressModeType::eClamp;
    Renderer::SetPixelStageSamplers(2, 1, &ClampSampler);
    Renderer::SetPixelStageShaderResources(0, kGBufferNum, WaterSurfaceGBufferTextures_.data());
    Renderer::SetPixelStageShaderResources(kGBufferNum, kGBufferNum, GBufferTextures_.data());
    Renderer::SetPixelStageShaderResources(kGBufferNum + kGBufferNum, 1, &ShadowMap_.GetDepthTexture());
    Renderer::SetPixelStageShaderResources(kGBufferNum + kGBufferNum + 1, 1, &SkyboxTexture_);
    Renderer::SetPixelStageConstantBuffers(0, 1, &CameraDataConstantBuffer_);
    Renderer::SetPixelStageConstantBuffers(1, 1, &LightDataConstantBuffer_);
    Renderer::SetPixelStageConstantBuffers(2, 1, &ShadowDataConstantBuffer_);
    Renderer::SetPixelStageConstantBuffers(3, 1, &RayMarchConstantBuffer_);
  }

  //  Renderer2Dのステートの設定
  {
    Renderer2D::SetPixelStageShaderResources(2, 1, &DepthTexture_.GetDepthTexture());
    Renderer2D::SetPixelStageConstantBuffers(0, 1, &CameraDataConstantBuffer_);
    Renderer2D::SetPixelStageConstantBuffers(1, 1, &DepthOfFieldConstantBuffer_);
  }

  //  Renderer3Dのステートの設定
  {
    Renderer3D::SetHullStageConstantBuffers(0, 1, &CameraDataConstantBuffer_);

    Renderer3D::SetDomainStageShaderResources(0, 1, &TerrainHeightMap_);
    Renderer3D::SetDomainStageConstantBuffers(0, 1, &CameraDataConstantBuffer_);
    Renderer3D::SetDomainStageConstantBuffers(1, 1, &LightDataConstantBuffer_);
    Renderer3D::SetDomainStageConstantBuffers(2, 1, &WaterSurfaceWaveDatasConstantBuffer_);
    Renderer3D::SetDomainStageConstantBuffers(3, 1, &WaterSurfaceDataConstantBuffer_);

    Renderer3D::SetPixelStageShaderResources(2, 1, &SkyboxTexture_);
    Renderer3D::SetPixelStageShaderResources(3, kTerrainTextureNum, TerrainTextures_.data());
    Renderer3D::SetPixelStageShaderResources(3 + kTerrainTextureNum, 1, &TerrainNormalMap_);
    Renderer3D::SetPixelStageShaderResources(3 + kTerrainTextureNum + 1, 1, &TerrainHeightMap_);
    Renderer3D::SetPixelStageShaderResources(3 + kTerrainTextureNum + 2, kWaterSurfaceNormalMapNum, WaterSurfaceNormalMaps_.data());
    Renderer3D::SetPixelStageConstantBuffers(0, 1, &TerrainDataConstantBuffer_);
    Renderer3D::SetPixelStageConstantBuffers(1, 1, &WaterSurfaceDataConstantBuffer_);
  }

  //  Computerのステートの設定
  {
    Computer::SetShaderResources(0, 1, &TerrainTexcoordMap_);
    Computer::SetConstantBuffers(0, 1, &TerrainUpdateDataConstantBuffer_);
    Computer::SetUnorderedAccessObjects(0, 1, &TerrainHeightMap_);
    Computer::SetUnorderedAccessObjects(1, 1, &TerrainNormalMap_);
  }
}

void SceneOcean::Update()
{
  //  更新
  {
    TerrainUpdateData& TerrainUpdateData = TerrainUpdateDataConstantBuffer_.GetData();
    WaterSurfaceData& WaterSurfaceData = WaterSurfaceDataConstantBuffer_.GetData();
    CameraData& CameraData = CameraDataConstantBuffer_.GetData();

    ImGui::Begin("SceneOcean", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    {
      ImGui::SetWindowPos(ImGuiHelper::kSceneWindowPos);
      ImGui::SetWindowSize(kSceneWindowSize);
      if (ImGui::Checkbox("Wireframe", &isWireframe_))
      {
        RasterizerState_ = (isWireframe_ ? RasterizerState::kWireframeCullNone : RasterizerState::kSolidCullNone);
      }
      if (ImGui::TreeNode("Camera"))
      {
        float FocalLength = CameraData.FocalLength;
        if (ImGui::SliderFloat("FocalLength", &FocalLength, 0.0f, 1.0f) && 0.0f <= FocalLength && FocalLength <= 1.0f)
        {
          CameraData.FocalLength = FocalLength;
        }
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("DirectinalLight"))
      {
        LightData& LightData = LightDataConstantBuffer_.GetData();
        if (ImGui::DragFloat3("Position", &DirectionalLightPosition_))
        {
          UpdateLightViewProjecion();
        }
        ImGui::Text(std::string("Direction:" + std::to_string(LightData.DirectionalLight.Direction)).c_str());
        ImGui::DragFloat("Itensity", &LightData.DirectionalLight.Itensity, 0.01f);
        ImGui::ColorEdit3("Color", &LightData.DirectionalLight.Color);
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("Shadow"))
      {
        ShadowData& ShadowData = ShadowDataConstantBuffer_.GetData();
        ImGui::ColorEdit3("Color", &ShadowData.Shadow);
        ImGui::DragFloat("Bias", &ShadowData.Shadow.Alpha, 1.0f, 0.0f, 0.0f, "%.6f");
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("Terrain"))
      {
#if DEBUG| _DEBUG
        if (ImGui::Button("Reload HeightMapUpdateCS"))
        {
          TerrainHeightMapUpdateComputeShader_ = ComputeShader(kTerrainHeightMapUpdateComputeShaderFilePath);
        }
        if (ImGui::Button("Reload TerrainPS"))
        {
          TerrainPixelShader_ = PixelShader(kTerrainPixelShaderFilePath, kTerrainPixelShaderEntryPoint);
        }
#endif
        if (ImGui::Button("Clear HeightMap"))
        {
          Renderer::Clear(TerrainHeightMap_);
          Renderer::Clear(TerrainNormalMap_, Palette::NormalMap);
        }
        ImGui::SliderFloat("BlushSize", &TerrainUpdateData.BlushSize, 0.1f, 150.0f);
        ImGui::SliderFloat("BlushHardness", &TerrainUpdateData.BlushHardness, -1.0f, 1.0f);
        TerrainData& TerrainData = TerrainDataConstantBuffer_.GetData();
        int TextureLoopNum = static_cast<int>(TerrainData.TextureLoopNum);
        if (ImGui::InputInt("TextureLoopNum", &TextureLoopNum) && TextureLoopNum > 0)
        {
          TerrainData.TextureLoopNum = TextureLoopNum;
        }
        ImGui::DragFloat("SandThreshold", &TerrainData.SandThreshold, 0.01f);
        ImGui::DragFloat("RockThreshold", &TerrainData.RockThreshold, 0.01f);
        ImGui::DragFloat("SlopeThreshold", &TerrainData.SlopeThreshold, 0.01f);
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("WaterSurface"))
      {
#if defined _DEBUG | DEBUG
        if (ImGui::Button("Reload WaterSurfaceDS"))
        {
          WaterSurfaceDomainShader_ = DomainShader(kWaterSurfaceDomainShaderFilePath);
        }
        if (ImGui::Button("Reload WaterSurfacePS"))
        {
          WaterSurfacePixelShader_ = PixelShader(kWaterSurfacePixelShaderFilePath);
        }
        if (ImGui::Button("Reload WaterSurfaceRefractionPS"))
        {
          WaterSurfaceRefractionPixelShader_ = PixelShader(kWaterSurfaceRefractionPixelShaderFilePath);
        }
        if (ImGui::Button("Reload WaterSurfaceReflectionPS"))
        {
          WaterSurfaceReflectionPixelShader_ = PixelShader(kWaterSurfaceReflectionPixelShaderFilePath);
        }
#endif
        ImGui::Checkbox("isUpdate", &isWaterSurfaceUpdate_);
        int WaveNum = static_cast<int>(WaterSurfaceData.WaveNum);
        if (ImGui::SliderInt("WaveNum", &WaveNum, 1, kMaxWaveNum) && (WaveNum > 0 && WaveNum <= kMaxWaveNum))
        {
          WaterSurfaceData.WaveNum = WaveNum;
        }
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("RayMarch"))
      {
        RayMarchData& RayMarchData = RayMarchConstantBuffer_.GetData();

        ImGui::SliderFloat("MaxStep", &RayMarchData.MaxStep, 0.0f, 1.0f);

        int SampleNum = static_cast<int>(RayMarchData.SampleNum);
        if (ImGui::SliderInt("SampleNum", &SampleNum, 1, kMaxRayMarchNum) && (SampleNum > 0 && SampleNum <= kMaxRayMarchNum))
        {
          RayMarchData.SampleNum = SampleNum;
        }

        RayMarchData.Step = RayMarchData.MaxStep / RayMarchData.SampleNum;

        ImGui::TreePop();
      }
    }
    ImGui::End();

    if (isWaterSurfaceUpdate_)
    {
      WaterSurfaceData.Time += System::GetDeltaTime();
    }

    if (!ImGui::IsAnyItemActive())
    {
      if (Input::isPressed(Input::Mouse::ButtonLeft))
      {
        Renderer::Clear(TerrainTexcoordMap_, kTerrainTexcoordMapClearColor);
        Renderer::Clear(TerrainTexcoordDepthTexture_);

        Renderer::SetRenderTexture(TerrainTexcoordMap_, TerrainTexcoordDepthTexture_);
        Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);
        Renderer3D::SetStaticMeshShaders(TerrainVertexShader_, TerrainHullShader_, TerrainTexcoordMapDomainShader_, TerrainTexcoordMapPixelShader_);
        Renderer3D::SetRasterizerState(RasterizerState::kSolidCullBack);

        DrawTerrain();
      }

      if (Input::isPress(Input::Mouse::ButtonLeft))
      {
        TerrainUpdateData.MousePosition = Input::Mouse::GetPos();

        if (TerrainUpdateData.MousePosition.x > 0 && TerrainUpdateData.MousePosition.y > 0
          && TerrainUpdateData.MousePosition.x < static_cast<int>(Constants::kDefaultWindowSize.x) && TerrainUpdateData.MousePosition.y < static_cast<int>(Constants::kDefaultWindowSize.y))
        {
          //  ハイトマップの更新
          {
            Computer::SetShader(TerrainHeightMapUpdateComputeShader_);
            Computer::Dispatch(1, 1, 1);
          }

          //  法線マップの更新
          {
            Computer::SetShader(TerrainNormalMapUpdateComputeShader_);
            Computer::Dispatch(kTerrainNormalMapDispatchNum);
          }
        }
      }
      else
      {
        FreeCamera(&Camera_);
        Renderer3D::SetCamera(Camera_);

        const vdl::Matrix View = Camera_.View();

        CameraData.EyePostion = Camera_.Position;
        CameraData.ViewProjection = View * Camera_.Projection(Constants::kDefaultWindowSize);
        CameraData.InverseViewProjection = CameraData.ViewProjection.Inverse();
        CameraData.ViewVector = Camera_.ViewVector().Normalize();

        Camera_.isPerspective = false;
        DepthOfFieldConstantBuffer_.GetData().OrthographicViewProjection = View * Camera_.Projection(Constants::kDefaultWindowSize);
        Camera_.isPerspective = true;
      }
    }
  }

  //  画面のクリア
  {
    for (uint i = 0; i < kGBufferNum; ++i)
    {
      Renderer::Clear(GBufferTextures_[i], kGBufferClearColors[i]);
      Renderer::Clear(WaterSurfaceGBufferTextures_[i], kGBufferClearColors[i]);
    }
    Renderer::Clear(DepthTexture_);
    Renderer::Clear(ShadowMap_);

    for (uint i = 0; i < kLightPassOutNum; ++i)
    {
      Renderer::Clear(LightPassRenderTextures_[i]);
    }
    for (uint i = 0; i < kShrinkBuffeNum; ++i)
    {
      Renderer::Clear(ShrinkBuffers_[i]);
    }
  }

  //  ShadowPass
  {
    Renderer::SetRenderTexture(RenderTexture(), ShadowMap_);

    Viewport Viewport = { float2(0.0f), kShadowMapSize };
    Renderer3D::SetViewport(Viewport);
    Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);
    Renderer3D::SetStaticMeshShaders(TerrainVertexShader_, TerrainHullShader_, TerrainShadowDomainShader_, PixelShader());
    Renderer3D::SetRasterizerState(RasterizerState::kSolidCullBack);

    DrawTerrain();

    Viewport.Size = Constants::kDefaultWindowSize;
    Renderer3D::SetViewport(Viewport);
  }

  //  GBufferPass
  {
    Renderer::SetRenderTextures(GBufferTextures_, DepthTexture_);

    //  地面の描画
    {
      Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);
      Renderer3D::SetStaticMeshShaders(TerrainVertexShader_, TerrainHullShader_, TerrainDomainShader_, TerrainPixelShader_);
      Renderer3D::SetRasterizerState(RasterizerState_);

      DrawTerrain();
    }
  }

  //  空の描画
  {
    Renderer::SetRenderTexture(LightPassRenderTextures_[0], DepthTexture_);

    Renderer3D::SetTopology(TopologyType::eDefaultMesh);
    Renderer3D::SetStaticMeshShaders(SkyboxVertexShader_, SkyboxPixelShader_);
    Renderer3D::SetRasterizerState(RasterizerState::kSolidCullFront);

    Renderer3D::Draw(Sphere_, Matrix::Scale(kSkyboxScale));
  }

  //  GBufferPass(水面)
  {
    Renderer::SetRenderTextures(WaterSurfaceGBufferTextures_, DepthTexture_);

    Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);
    Renderer3D::SetStaticMeshShaders(WaterSurfaceVertexShader_, WaterSurfaceHullShader_, WaterSurfaceDomainShader_, WaterSurfacePixelShader_);
    Renderer3D::SetRasterizerState(RasterizerState_);

    for (uint y = 0; y < kRetangleNum.y; ++y)
    {
      for (uint x = 0; x < kRetangleNum.x; ++x)
      {
        Renderer3D::Draw(Rectangle_, Matrix::Translate({ (x - kRetangleHalfNum.x) * kRetangleSize.x, kWaterSurfaceHeight, (y - kRetangleHalfNum.y) * kRetangleSize.y }), ColorF(Palette::LightSkyBlue, 0.25f));
      }
    }
  }

  Renderer::SetRenderTextures(LightPassRenderTextures_, DepthStencilTexture());
  //Renderer::SetRenderTextures(OutputRenderTextures_, DepthStencilTexture());

  //  Refraction
  {
    Renderer::SetPixelShader(WaterSurfaceRefractionPixelShader_);
    Renderer::SetBlendState(BlendState::kDefault);

    Renderer::Draw(3);
  }

  //  Reflection
  {
    Renderer::SetPixelShader(WaterSurfaceReflectionPixelShader_);
    Renderer::SetBlendState(BlendState::kAdd);

    Renderer::Draw(3);
  }

  //  GaussianBlur
  {
    Texture SrcTexture = LightPassRenderTextures_[0];
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

    Viewport.Size = Constants::kDefaultWindowSize;
    Renderer2D::SetViewport(Viewport);
  }

  Renderer::SetRenderTexture(SwapchainRenderTexture_, DepthStencilTexture());
  Renderer2D::SetPixelShader(DepthOfFieldPixelShader_);
  Renderer2D::SetPixelStageShaderResources(1, 1, &ShrinkBuffers_[kShrinkBuffeNum - 1]);
  Renderer2D::Draw(LightPassRenderTextures_[0]);
  Texture Texture;
  Renderer2D::SetPixelStageShaderResources(1, 1, &Texture);

  ////  Bloom
  //{
  //  Renderer::SetRenderTexture(SwapchainRenderTexture_, DepthStencilTexture());
  //  Renderer2D::SetBlendState(BlendState::kAdd);
  //
  //  for (uint i = 0; i < kShrinkBuffeNum; ++i)
  //  {
  //    Renderer2D::Draw(ShrinkBuffers_[i], 0.0f, Constants::kDefaultWindowSize);
  //  }
  //}

  ImGui::Begin("RenderingFlow", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
  {
    ImGui::SetWindowPos(ImGuiHelper::kRenderingFlowWindowPos);
    ImGui::SetWindowSize(ImGuiHelper::kRenderingFlowWindowSize);
    if (ImGui::TreeNode("HeightMapUpdatePass"))
    {
      ImGuiHelper::DrawRenderTexture("TexcoordMap", TerrainTexcoordMap_, kHeightMapDisplaySize);
      ImGuiHelper::DrawUnorderedAccessTexture("HeightMap", TerrainHeightMap_, kHeightMapDisplaySize);
      ImGuiHelper::DrawUnorderedAccessTexture("NormalMap", TerrainNormalMap_, kHeightMapDisplaySize);

      ImGui::TreePop();
    }
    if (ImGui::TreeNode("GBufferPass"))
    {
      for (uint i = 0; i < kGBufferNum; ++i)
      {
        ImGuiHelper::DrawRenderTexture(kGBufferNames[i], GBufferTextures_[i], ImGuiHelper::kGBufferDisplaySize);
      }

      ImGui::TreePop();
    }
    if (ImGui::TreeNode("WaterSurface"))
    {
      for (uint i = 0; i < kGBufferNum; ++i)
      {
        ImGuiHelper::DrawRenderTexture(kGBufferNames[i], WaterSurfaceGBufferTextures_[i], ImGuiHelper::kGBufferDisplaySize);
      }
      ImGui::TreePop();
    }
    if (ImGui::TreeNode("ShadowPass"))
    {
      ImGuiHelper::DrawDepthTexture("ShadowMap", ShadowMap_, kHeightMapDisplaySize);

      ImGui::TreePop();
    }
    if (ImGui::TreeNode("LightPass"))
    {
      for (uint i = 0; i < kLightPassOutNum; ++i)
      {
        ImGuiHelper::DrawRenderTexture(kLightPassOutNames[i], LightPassRenderTextures_[i], ImGuiHelper::kGBufferDisplaySize);
      }

      ImGui::TreePop();
    }
    if (ImGui::TreeNode("BlurPass"))
    {
      for (uint i = 0; i < kShrinkBuffeNum; ++i)
      {
        ImGuiHelper::DrawRenderTexture(std::string("ShirnkBuffer" + std::to_string(i)).c_str(), ShrinkBuffers_[i], ImGuiHelper::kGBufferDisplaySize);
      }
      ImGui::TreePop();
    }
  }
  ImGui::End();
}

//--------------------------------------------------

void SceneOcean::UpdateLightViewProjecion()
{
  Camera Light = Camera(DirectionalLightPosition_);
  Light.isPerspective = false;
  Light.Near = kCameraNear;
  Light.Far = kCameraFar;

  LightData& LightData = LightDataConstantBuffer_.GetData();
  LightData.DirectionalLight.Direction = -DirectionalLightPosition_.Normalize();
  LightData.ViewProjection = Light.View() * Light.Projection(Constants::kDefaultWindowSize / 4.0f);
}

void SceneOcean::DrawTerrain()const
{
  for (uint y = 0; y < kRetangleNum.y; ++y)
  {
    for (uint x = 0; x < kRetangleNum.x; ++x)
    {
      Renderer3D::Draw(Rectangle_, Matrix::Translate({ (x - kRetangleHalfNum.x) * kRetangleSize.x, 0.0f, (y - kRetangleHalfNum.y) * kRetangleSize.y }));
    }
  }
}
