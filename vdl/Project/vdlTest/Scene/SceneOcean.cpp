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

  //  LightPassの初期化
  {

  }

  //  PostProcessの初期化
  {

  }

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

    ImGui::Begin("SceneOcean", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    {
      ImGui::SetWindowPos(ImGuiHelper::kSceneWindowPos);
      ImGui::SetWindowSize(kSceneWindowSize);
      if (ImGui::Checkbox("Wireframe", &isWireframe_))
      {
        RasterizerState_ = (isWireframe_ ? RasterizerState::kWireframeCullNone : RasterizerState::kSolidCullNone);
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
//#if defined _DEBUG | DEBUG
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
//#endif
        ImGui::Checkbox("isUpdate", &isWaterSurfaceUpdate_);
        int WaveNum = static_cast<int>(WaterSurfaceData.WaveNum);
        if (ImGui::SliderInt("WaveNum", &WaveNum, 1, kMaxWaveNum) && (WaveNum > 0 && WaveNum <= kMaxWaveNum))
        {
          WaterSurfaceData.WaveNum = WaveNum;
        }
        ImGui::TreePop();
      }
    }
    ImGui::End();

    if (isWaterSurfaceUpdate_)
    {
      WaterSurfaceData.Time += System::GetDeltaTime();
    }

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

      CameraData& CameraData = CameraDataConstantBuffer_.GetData();
      CameraData.EyePostion = Camera_.Position;
      CameraData.ViewProjection = Camera_.View() * Camera_.Projection(Constants::kDefaultWindowSize);
      CameraData.InverseViewProjection = CameraData.ViewProjection.Inverse();
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
    Renderer::SetRenderTexture(SwapchainRenderTexture_, DepthTexture_);

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

  //  Refraction
  {
    Renderer::SetRenderTexture(SwapchainRenderTexture_, DepthStencilTexture());
  
    Renderer::SetPixelShader(WaterSurfaceRefractionPixelShader_);
    Renderer::SetBlendState(BlendState::kDefault);
  
    Renderer::Draw(3);
  }


  //  Reflection
  {
    Renderer::SetRenderTexture(SwapchainRenderTexture_, DepthStencilTexture());

    Renderer::SetPixelShader(WaterSurfaceReflectionPixelShader_);
    Renderer::SetBlendState(BlendState::kAdd);

    Renderer::Draw(3);
  }

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

//void SceneOcean::Initialize()
//{
//  StaticModelData ModelData;
//  ModelData.Vertices.resize(4);
//  {
//    ModelData.Vertices[0] = { { +kRetangleHalfSize.x, 0.0f, +kRetangleHalfSize.y }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
//    ModelData.Vertices[1] = { { -kRetangleHalfSize.x, 0.0f, +kRetangleHalfSize.y }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
//    ModelData.Vertices[2] = { { +kRetangleHalfSize.x, 0.0f, -kRetangleHalfSize.y }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };
//    ModelData.Vertices[3] = { { -kRetangleHalfSize.x, 0.0f, -kRetangleHalfSize.y }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
//  }
//  ModelData.Indices = { 0, 1, 2, 3 };
//  ModelData.MeshDatas.resize(1);
//  {
//    ModelData.MeshDatas[0].IndexCount = 4;
//    ModelData.MeshDatas[0].Material.Diffuse = Palette::Diffuse;
//  }
//
//  Rectangle_ = ModelData;
//
//  Camera_ = Camera(float3(0.0f, 15.0f, -60.0f));
//  Camera_.Near = 5.0f;
//  Camera_.Far = 500.0f;
//  DirectionalLightPosition_ = float3(0.0f, 150.0f, -350.0f);
//  SwapchainRenderTexture_ = Window::GetRenderTexture();
//  ShadowMap_ = DepthStencilTexture(kShadowMapSize, FormatType::eD32_Float);
//  RasterizerState_ = RasterizerState::kSolidCullNone;
//
//  LightData& LightData = LightDataConstantBuffer_.GetData();
//  {
//    LightData.Ambient = Palette::DimGray;
//    LightData.DirectionalLight.Direction = -DirectionalLightPosition_.Normalize();
//    LightData.DirectionalLight.Color = Palette::White;
//    LightData.DirectionalLight.Itensity = 3.0f;
//  }
//
//  UpdateLightViewProjecion();
//
//  CameraData& CameraData = CameraDataConstantBuffer_.GetData();
//  {
//    CameraData.EyePostion = Camera_.Position;
//    CameraData.ViewProjection = Camera_.View() * Camera_.Projection(Constants::kDefaultWindowSize);
//  }
//
//  //  DeferredGBufferの初期化
//  {
//    GBufferTextures_[static_cast<uint>(GBufferType::eDiffuse)] = RenderTexture(kGBufferSize, FormatType::eR8G8B8A8_Unorm);
//    GBufferTextures_[static_cast<uint>(GBufferType::eNormal)] = RenderTexture(kGBufferSize, FormatType::eR8G8B8A8_Unorm);
//    DepthTexture_ = DepthStencilTexture(kGBufferSize, FormatType::eD32_Float);
//    CopyDepthTexture_ = RenderTexture(kGBufferSize, FormatType::eR32_Float);
//  }
//
//  //  LightPassの初期化
//  {
//    LightPassVertexShader_ = VertexShader("Shader/Option/FullScreenTriangleVS.hlsl", InputLayoutType::eNone);
//    LightPassPixelShader_ = PixelShader("Shader/Ocean/LightPassPS.hlsl");
//
//    LightPassData& LightPassData = LightPassDataConstantBuffer_.GetData();
//    {
//      LightPassData.Shadow = ColorF(Palette::DimGray, 0.0001f);
//      LightPassData.InverseViewProjection = CameraData.ViewProjection.Inverse();
//    }
//  }
//
//  //  空の初期化
//  {
//    Sphere_ = StaticModelData::Sphere(20, 20);
//    SkyboxTexture_ = CubeTexture("Data/skybox.png");
//
//    SkyboxVertexShader_ = VertexShader("Shader/Skybox/SkyboxVS.hlsl", InputLayoutType::eStaticMesh);
//    SkyboxPixelShader_ = PixelShader("Shader/Skybox/SkyboxPS.hlsl");
//  }
//
//  //  地面の初期化
//  {
//    TerrainTexcoordMap_ = RenderTexture(kGBufferSize, FormatType::eR16G16_Float);
//    TerrainHeightMap_ = UnorderedAccessTexture(kHeightMapSize, FormatType::eR32_Float);
//    TerrainNormalMap_ = UnorderedAccessTexture(kHeightMapSize, FormatType::eR8G8B8A8_Unorm);
//    TerrainTexcoordDepthTexture_ = DepthStencilTexture(kGBufferSize, FormatType::eD32_Float);
//    TerrainHeightMapUpdateComputeShader_ = ComputeShader(kTerrainHeightMapUpdateComputeShaderFilePath);
//    TerrainNormalMapUpdateComputeShader_ = ComputeShader(kTerrainNormalMapUpdateComputeShaderFilePath);
//    Renderer::Clear(TerrainTexcoordMap_, kTerrainTexcoordMapClearColor);
//    Renderer::Clear(TerrainNormalMap_, Palette::NormalMap);
//
//    TerrainUpdateData& TerrainUpdateData = TerrainUpdateDataConstantBuffer_.GetData();
//    {
//      TerrainUpdateData.BlushSize = 15.0f;
//      TerrainUpdateData.BlushHardness = 1.0f;
//    }
//
//    TerrainData& TerrainData = TerrainDataConstantBuffer_.GetData();
//    {
//      TerrainData.TextureLoopNum = 5;
//      TerrainData.SandThreshold = 5.0f;
//      TerrainData.RockThreshold = 7.5f;
//      TerrainData.SlopeThreshold = 10.0f;
//    }
//
//    TerrainTextures_[0] = Texture("Data/Ocean/Terrain/sand_diffuse.png");
//    TerrainTextures_[1] = Texture("Data/Ocean/Terrain/terrain_rock4.png");
//    TerrainTextures_[2] = Texture("Data/Ocean/Terrain/terrain_slope.png");
//    TerrainTextures_[3] = Texture("Data/Ocean/Terrain/terrain_grass.png");
//    TerrainVertexShader_ = VertexShader("Shader/Ocean/Terrain/TerrainVS.hlsl", InputLayoutType::eStaticMesh);
//    TerrainHullShader_ = HullShader("Shader/Ocean/Terrain/TerrainHS.hlsl");
//    TerrainDomainShader_ = DomainShader("Shader/Ocean/Terrain/TerrainDS.hlsl", "Color");
//    TerrainPixelShader_ = PixelShader(kTerrainPixelShaderFilePath, kTerrainPixelShaderEntryPoint);
//    TerrainShadowDomainShader_ = DomainShader("Shader/Ocean/Terrain/TerrainDS.hlsl", "Shadow");
//    TerrainTexcoordMapDomainShader_ = DomainShader("Shader/Ocean/Terrain/TerrainDS.hlsl", "Texcoord");
//    TerrainTexcoordMapPixelShader_ = PixelShader("Shader/Ocean/Terrain/TerrainPS.hlsl", "HeightMapUpdatePrePass");
//  }
//
//  //  水面の初期化
//  {
//    WaterSurfaceRenderTextures_[static_cast<uint>(WaterSurfaceGBufferType::eDiffuse)] = RenderTexture(kGBufferSize, FormatType::eR8G8B8A8_Unorm);
//    WaterSurfaceRenderTextures_[static_cast<uint>(WaterSurfaceGBufferType::eNormal)] = RenderTexture(kGBufferSize, FormatType::eR8G8B8A8_Unorm);
//    WaterSurfaceRenderTextures_[static_cast<uint>(WaterSurfaceGBufferType::eDepth)] = RenderTexture(kGBufferSize, FormatType::eR32_Float);
//
//    WaterSurfaceVertexShader_ = VertexShader("Shader/Ocean/WaterSurface/WaterSurfaceVS.hlsl", InputLayoutType::eStaticMesh);
//    WaterSurfaceHullShader_ = HullShader("Shader/Ocean/WaterSurface/WaterSurfaceHS.hlsl");
//    WaterSurfaceDomainShader_ = DomainShader(kWaterSurfaceDomainShaderFilePath);
//    WaterSurfacePixelShader_ = PixelShader(kWaterSurfacePixelShaderFilePath);
//
//    for (uint i = 0; i < kWaterSurfaceNormalMapNum; ++i)
//    {
//      char FIlePath[256];
//      sprintf_s(FIlePath, "Data/Ocean/WaterSurface/WaterNormalMap%d.png", i);
//      WaterSurfaceNormalMaps_[i] = FIlePath;
//    }
//
//    std::array<Wave, kMaxWaveNum>& Waves = WaterSurfaceWaveDatasConstantBuffer_.GetData();
//    {
//      for (auto& Wave : Waves)
//      {
//        const float Radian = Random::Range(0.0f, Math::ToRadian(360.0f)) * 0.3f;
//        Wave.Direction = float3(sinf(Radian), 0.0f, cosf(Radian));
//        Wave.Length = Random::Range(kWaveMinLength, kWaveMaxLength);
//        Wave.Speed = Random::Range(5.0f, 10.0f);
//        //Wave.Speed = 1.0f;
//        Wave.Steepness = Random::Range(0.15f, 1.0f);
//        //Wave.Steepness = 0.8f;
//        Wave.Amplitude = 0.03f + powf(2.0f, Random::Range(0.0f, 2.0f)) * Random::Range(0.0125f, 0.025f);
//      }
//    }
//
//    WaterSurfaceGerstnerData& WaterSurfaceGerstnerData = WaterSurfaceGerstnerConstantBuffer_.GetData();
//    {
//      WaterSurfaceGerstnerData.Time = 0.0f;
//      WaterSurfaceGerstnerData.WaveNum = kMaxWaveNum;
//    }
//
//    WaterSurfaceReflectionRenderTexture_ = RenderTexture(kGBufferSize, FormatType::eR8G8B8A8_Unorm);
//    WaterSurfaceReflectionVertexShader_ = LightPassVertexShader_;
//    WaterSurfaceReflectionPixelShader_ = PixelShader(kWaterSurfaceReflectionPixelShaderFilePath);
//  }
//
//  Sampler ShadowSampler = Sampler::kDefault3D;
//  ShadowSampler.AddressModeU = ShadowSampler.AddressModeV = ShadowSampler.AddressModeW = AddressModeType::eBorder;
//  ShadowSampler.BorderColor = BorderColorType::eWhite;
//
//  //  Rendererのステートの設定
//  {
//    Renderer::SetTopology(TopologyType::eTriangleStrip);
//
//    Renderer::SetPixelStageSamplers(1, 1, &ShadowSampler);
//
//    Sampler ClampSampler = Sampler::kDefault3D;
//    ShadowSampler.AddressModeU = ShadowSampler.AddressModeV = ShadowSampler.AddressModeW = AddressModeType::eBorder;
//    ClampSampler.BorderColor = BorderColorType::eWhite;
//    Renderer::SetPixelStageSamplers(2, 1, &ClampSampler);
//
//    Renderer::SetPixelStageConstantBuffers(0, 1, &LightDataConstantBuffer_);
//    Renderer::SetPixelStageConstantBuffers(1, 1, &LightPassDataConstantBuffer_);
//    Renderer::SetPixelStageConstantBuffers(2, 1, &LightViewProjectionConstantBuffer_);
//    Renderer::SetPixelStageConstantBuffers(3, 1, &CameraDataConstantBuffer_);
//  }
//
//  //  Renderer3Dのステートの設定
//  {
//    Renderer3D::SetHullStageConstantBuffers(0, 1, &CameraDataConstantBuffer_);
//
//    Renderer3D::SetDomainStageShaderResources(0, 1, &TerrainHeightMap_);
//    Renderer3D::SetDomainStageConstantBuffers(1, 1, &WaterSurfaceWaveDatasConstantBuffer_);
//    Renderer3D::SetDomainStageConstantBuffers(2, 1, &WaterSurfaceGerstnerConstantBuffer_);
//
//    Renderer3D::SetPixelStageSamplers(1, 1, &ShadowSampler);
//    Renderer3D::SetPixelStageShaderResources(2, 1, &SkyboxTexture_);
//    Renderer3D::SetPixelStageShaderResources(3, kTerrainTextureNum, TerrainTextures_.data());
//    Renderer3D::SetPixelStageShaderResources(3 + kTerrainTextureNum, 1, &TerrainNormalMap_);
//    Renderer3D::SetPixelStageShaderResources(3 + kTerrainTextureNum + 1, 1, &TerrainHeightMap_);
//    Renderer3D::SetPixelStageShaderResources(9, kWaterSurfaceNormalMapNum, WaterSurfaceNormalMaps_.data());
//    Renderer3D::SetPixelStageConstantBuffers(0, 1, &TerrainDataConstantBuffer_);
//    Renderer3D::SetPixelStageConstantBuffers(1, 1, &WaterSurfaceGerstnerConstantBuffer_);
//    //Renderer3D::SetPixelStageConstantBuffers(2, 1, &LightDataConstantBuffer_);
//    //Renderer3D::SetPixelStageConstantBuffers(3, 1, &LightPassDataConstantBuffer_);
//    //Renderer3D::SetPixelStageConstantBuffers(4, 1, &LightViewProjectionConstantBuffer_);
//    //Renderer3D::SetPixelStageConstantBuffers(5, 1, &CameraDataConstantBuffer_);
//  }
//
//  //  Computerのステートの設定
//  {
//    Computer::SetShaderResources(0, 1, &TerrainTexcoordMap_);
//    Computer::SetConstantBuffers(0, 1, &TerrainUpdateDataConstantBuffer_);
//    Computer::SetUnorderedAccessObjects(0, 1, &TerrainHeightMap_);
//    Computer::SetUnorderedAccessObjects(1, 1, &TerrainNormalMap_);
//  }
//}
//
//void SceneOcean::Update()
//{
//  //  更新
//  {
//    LightPassData& LightPassData = LightPassDataConstantBuffer_.GetData();
//    TerrainUpdateData& TerrainUpdateData = TerrainUpdateDataConstantBuffer_.GetData();
//
//    ImGui::Begin("SceneOcean", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
//    {
//      ImGui::SetWindowPos(ImGuiHelper::kSceneWindowPos);
//      ImGui::SetWindowSize(kSceneWindowSize);
//      if (ImGui::Checkbox("Wireframe", &isWireframe_))
//      {
//        RasterizerState_ = (isWireframe_ ? RasterizerState::kWireframeCullNone : RasterizerState::kSolidCullNone);
//      }
//      if (ImGui::TreeNode("DirectinalLight"))
//      {
//        LightData& LightData = LightDataConstantBuffer_.GetData();
//        if (ImGui::DragFloat3("Position", &DirectionalLightPosition_))
//        {
//          LightData.DirectionalLight.Direction = -DirectionalLightPosition_.Normalize();
//          UpdateLightViewProjecion();
//        }
//        ImGui::Text(std::string("Direction:" + std::to_string(LightData.DirectionalLight.Direction)).c_str());
//        ImGui::DragFloat("Itensity", &LightData.DirectionalLight.Itensity, 0.01f);
//        ImGui::ColorEdit3("Color", &LightData.DirectionalLight.Color);
//        ImGui::ColorEdit3("Ambient", &LightData.Ambient);
//        ImGui::TreePop();
//      }
//      if (ImGui::TreeNode("Shadow"))
//      {
//        ImGui::ColorEdit3("Color", &LightPassData.Shadow);
//        ImGui::DragFloat("Bias", &LightPassData.Shadow.Alpha, 1.0f, 0.0f, 0.0f, "%.6f");
//        ImGui::TreePop();
//      }
//      if (ImGui::TreeNode("Terrain"))
//      {
//#if DEBUG| _DEBUG
//        if (ImGui::Button("Reload HeightMapUpdateCS"))
//        {
//          TerrainHeightMapUpdateComputeShader_ = ComputeShader(kTerrainHeightMapUpdateComputeShaderFilePath);
//        }
//        if (ImGui::Button("Reload TerrainPS"))
//        {
//          TerrainPixelShader_ = PixelShader(kTerrainPixelShaderFilePath, kTerrainPixelShaderEntryPoint);
//        }
//#endif
//        if (ImGui::Button("Clear HeightMap"))
//        {
//          Renderer::Clear(TerrainHeightMap_);
//          Renderer::Clear(TerrainNormalMap_, Palette::NormalMap);
//        }
//        ImGui::SliderFloat("BlushSize", &TerrainUpdateData.BlushSize, 0.1f, 150.0f);
//        ImGui::SliderFloat("BlushHardness", &TerrainUpdateData.BlushHardness, -1.0f, 1.0f);
//        TerrainData& TerrainData = TerrainDataConstantBuffer_.GetData();
//        int TextureLoopNum = static_cast<int>(TerrainData.TextureLoopNum);
//        if (ImGui::InputInt("TextureLoopNum", &TextureLoopNum) && TextureLoopNum > 0)
//        {
//          TerrainData.TextureLoopNum = TextureLoopNum;
//        }
//        ImGui::DragFloat("SandThreshold", &TerrainData.SandThreshold, 0.01f);
//        ImGui::DragFloat("RockThreshold", &TerrainData.RockThreshold, 0.01f);
//        ImGui::DragFloat("SlopeThreshold", &TerrainData.SlopeThreshold, 0.01f);
//        ImGui::TreePop();
//      }
//      if (ImGui::TreeNode("WaterSurface"))
//      {
//#if defined _DEBUG | DEBUG
//        if (ImGui::Button("Reload WaterSurfaceDS"))
//        {
//          WaterSurfaceDomainShader_ = DomainShader(kWaterSurfaceDomainShaderFilePath);
//        }
//        if (ImGui::Button("Reload WaterSurfacePS"))
//        {
//          WaterSurfacePixelShader_ = PixelShader(kWaterSurfacePixelShaderFilePath);
//        }
//        if (ImGui::Button("Reload WaterSurfaceReflectionPS"))
//        {
//          WaterSurfaceReflectionPixelShader_ = PixelShader(kWaterSurfaceReflectionPixelShaderFilePath);
//        }
//#endif
//        ImGui::Checkbox("isUpdate", &isWaterSurfaceUpdate_);
//        WaterSurfaceGerstnerData& WaterSurfaceGerstnerData = WaterSurfaceGerstnerConstantBuffer_.GetData();
//        int WaveNum = static_cast<int>(WaterSurfaceGerstnerData.WaveNum);
//        if (ImGui::SliderInt("WaveNum", &WaveNum, 1, kMaxWaveNum) && (WaveNum > 0 && WaveNum <= kMaxWaveNum))
//        {
//          WaterSurfaceGerstnerData.WaveNum = WaveNum;
//        }
//        ImGui::TreePop();
//      }
//    }
//    ImGui::End();
//
//    if (isWaterSurfaceUpdate_)
//    {
//      WaterSurfaceGerstnerConstantBuffer_.GetData().Time += System::GetDeltaTime();
//    }
//
//    if (Input::isPressed(Input::Mouse::ButtonLeft))
//    {
//      Renderer::Clear(TerrainTexcoordMap_, kTerrainTexcoordMapClearColor);
//      Renderer::Clear(TerrainTexcoordDepthTexture_);
//
//      Renderer::SetRenderTexture(TerrainTexcoordMap_, TerrainTexcoordDepthTexture_);
//      Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);
//      Renderer3D::SetStaticMeshShaders(TerrainVertexShader_, TerrainHullShader_, TerrainTexcoordMapDomainShader_, TerrainTexcoordMapPixelShader_);
//      Renderer3D::SetRasterizerState(RasterizerState::kSolidCullBack);
//
//      DrawTerrain();
//    }
//
//    if (Input::isPress(Input::Mouse::ButtonLeft))
//    {
//      TerrainUpdateData.MousePosition = Input::Mouse::GetPos();
//
//      if (TerrainUpdateData.MousePosition.x > 0 && TerrainUpdateData.MousePosition.y
//        && TerrainUpdateData.MousePosition.x < static_cast<int>(Constants::kDefaultWindowSize.x) && TerrainUpdateData.MousePosition.y < static_cast<int>(Constants::kDefaultWindowSize.y))
//      {
//        //  ハイトマップの更新
//        {
//          Computer::SetShader(TerrainHeightMapUpdateComputeShader_);
//          Computer::Dispatch(1, 1, 1);
//        }
//
//        //  法線マップの更新
//        {
//          Computer::SetShader(TerrainNormalMapUpdateComputeShader_);
//          Computer::Dispatch(kTerrainNormalMapDispatchNum);
//        }
//      }
//    }
//    else
//    {
//      FreeCamera(&Camera_);
//      Renderer3D::SetCamera(Camera_);
//
//      CameraData& CameraData = CameraDataConstantBuffer_.GetData();
//      CameraData.EyePostion = Camera_.Position;
//      CameraData.ViewProjection = Camera_.View() * Camera_.Projection(Constants::kDefaultWindowSize);
//      LightPassData.InverseViewProjection = CameraData.ViewProjection.Inverse();
//    }
//  }
//
//  //  画面のクリア
//  {
//    for (uint i = 0; i < kGBufferNum; ++i)
//    {
//      Renderer::Clear(GBufferTextures_[i], kGBufferClearColor);
//    }
//    for (uint i = 0; i < kWaterSurfaceGBufferNum; ++i)
//    {
//      Renderer::Clear(WaterSurfaceRenderTextures_[i], kGBufferClearColor);
//    }
//    Renderer::Clear(WaterSurfaceReflectionRenderTexture_, kGBufferClearColor);
//    Renderer::Clear(DepthTexture_);
//    Renderer::Clear(CopyDepthTexture_);
//    Renderer::Clear(ShadowMap_);
//  }
//
//  //  ShadowPass
//  {
//    Renderer::SetRenderTexture(RenderTexture(), ShadowMap_);
//
//    Viewport Viewport = { float2(0.0f), kShadowMapSize };
//    Renderer3D::SetViewport(Viewport);
//    Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);
//    Renderer3D::SetStaticMeshShaders(TerrainVertexShader_, TerrainHullShader_, TerrainShadowDomainShader_, PixelShader());
//    Renderer3D::SetRasterizerState(RasterizerState::kSolidCullBack);
//    Renderer3D::SetDomainStageConstantBuffers(0, 1, &LightViewProjectionConstantBuffer_);
//
//    DrawTerrain();
//
//    Viewport.Size = Constants::kDefaultWindowSize;
//    Renderer3D::SetViewport(Viewport);
//  }
//
//  //  空の描画
//  {
//    Renderer::SetRenderTexture(SwapchainRenderTexture_, DepthStencilTexture());
//
//    Renderer3D::SetTopology(TopologyType::eDefaultMesh);
//    Renderer3D::SetStaticMeshShaders(SkyboxVertexShader_, SkyboxPixelShader_);
//    Renderer3D::SetRasterizerState(RasterizerState::kSolidCullFront);
//
//    Renderer3D::Draw(Sphere_, Matrix::Scale(500.0f));
//  }
//
//  //  GBufferPass
//  {
//    Renderer::SetRenderTextures(GBufferTextures_, DepthTexture_);
//
//    //  地面の描画
//    {
//      Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);
//      Renderer3D::SetStaticMeshShaders(TerrainVertexShader_, TerrainHullShader_, TerrainDomainShader_, TerrainPixelShader_);
//      Renderer3D::SetRasterizerState(RasterizerState_);
//      Renderer3D::SetDomainStageConstantBuffers(0, 1, &CameraDataConstantBuffer_);
//
//      DrawTerrain();
//    }
//  }
//
//  //  LightPass
//  {
//    Renderer::SetRenderTexture(SwapchainRenderTexture_, DepthStencilTexture());
//    Renderer::SetShaders(LightPassVertexShader_, LightPassPixelShader_);
//    Renderer::SetPixelStageShaderResources(0, kGBufferNum, GBufferTextures_.data());
//    Renderer::SetPixelStageShaderResources(kGBufferNum, 1, &DepthTexture_.GetDepthTexture());
//    Renderer::SetPixelStageShaderResources(kGBufferNum + 1, 1, &ShadowMap_.GetDepthTexture());
//
//    Renderer::Draw(3);
//  }
//
//  //  深度バッファのコピー
//  {
//    Renderer::SetRenderTexture(CopyDepthTexture_, DepthStencilTexture());
//
//    Renderer2D::Draw(DepthTexture_.GetDepthTexture());
//  }
//
//  //  水の描画
//  {
//    Renderer::SetRenderTextures(WaterSurfaceRenderTextures_, DepthTexture_);
//    Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);
//    Renderer3D::SetStaticMeshShaders(WaterSurfaceVertexShader_, WaterSurfaceHullShader_, WaterSurfaceDomainShader_, WaterSurfacePixelShader_);
//    Renderer3D::SetRasterizerState(RasterizerState_);
//
//    for (uint y = 0; y < kRetangleNum.y; ++y)
//    {
//      for (uint x = 0; x < kRetangleNum.x; ++x)
//      {
//        Renderer3D::Draw(Rectangle_, Matrix::Translate({ (x - kRetangleHalfNum.x) * kRetangleSize.x, kWaterSurfaceHeight, (y - kRetangleHalfNum.y) * kRetangleSize.y }), ColorF(Palette::LightSkyBlue, 0.25f));
//      }
//    }
//
//    //  反射
//    Renderer::SetRenderTexture(WaterSurfaceReflectionRenderTexture_, DepthStencilTexture());
//    Renderer::SetShaders(WaterSurfaceReflectionVertexShader_, WaterSurfaceReflectionPixelShader_);
//    Renderer::SetPixelStageShaderResources(0, 2, &WaterSurfaceRenderTextures_[1]);
//    //Renderer::SetPixelStageShaderResources(1, 1, &DepthTexture_.GetDepthTexture());
//    Renderer::SetPixelStageShaderResources(2, kGBufferNum, GBufferTextures_.data());
//    Renderer::SetPixelStageShaderResources(4, 1, &CopyDepthTexture_);
//    Renderer::SetPixelStageShaderResources(5, 1, &SkyboxTexture_);
//
//    Renderer::Draw(3);
//
//    Renderer::SetRenderTexture(SwapchainRenderTexture_, DepthStencilTexture());
//    Renderer::SetShaders(LightPassVertexShader_, LightPassPixelShader_);
//    Renderer::SetPixelStageShaderResources(0, 2, WaterSurfaceRenderTextures_.data());
//    Renderer::SetPixelStageShaderResources(2, 1, &DepthTexture_.GetDepthTexture());
//    Renderer::SetPixelStageShaderResources(3, 1, &ShadowMap_.GetDepthTexture());
//
//    Renderer::Draw(3);
//
//    Renderer2D::Draw(WaterSurfaceReflectionRenderTexture_, ColorF(1.0f, 1.0f, 1.0f, 0.25f));
//  }
//
//  ImGui::Begin("RenderingFlow", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
//  {
//    ImGui::SetWindowPos(ImGuiHelper::kRenderingFlowWindowPos);
//    ImGui::SetWindowSize(ImGuiHelper::kRenderingFlowWindowSize);
//    if (ImGui::TreeNode("HeightMapUpdatePass"))
//    {
//      ImGuiHelper::DrawRenderTexture("TexcoordMap", TerrainTexcoordMap_, kHeightMapDisplaySize);
//      ImGuiHelper::DrawUnorderedAccessTexture("HeightMap", TerrainHeightMap_, kHeightMapDisplaySize);
//      ImGuiHelper::DrawUnorderedAccessTexture("NormalMap", TerrainNormalMap_, kHeightMapDisplaySize);
//
//      ImGui::TreePop();
//    }
//    if (ImGui::TreeNode("GBufferPass"))
//    {
//      ImGuiHelper::DrawRenderTexture("Diffuse", GBufferTextures_[static_cast<uint>(GBufferType::eDiffuse)], ImGuiHelper::kGBufferDisplaySize);
//      ImGuiHelper::DrawRenderTexture("Normal", GBufferTextures_[static_cast<uint>(GBufferType::eNormal)], ImGuiHelper::kGBufferDisplaySize);
//      ImGuiHelper::DrawRenderTexture("Depth", CopyDepthTexture_, ImGuiHelper::kGBufferDisplaySize);
//
//      ImGui::TreePop();
//    }
//    if (ImGui::TreeNode("WaterSurface"))
//    {
//      ImGuiHelper::DrawRenderTexture("Diffuse", WaterSurfaceRenderTextures_[static_cast<uint>(WaterSurfaceGBufferType::eDiffuse)], ImGuiHelper::kGBufferDisplaySize);
//      ImGuiHelper::DrawRenderTexture("Normal", WaterSurfaceRenderTextures_[static_cast<uint>(WaterSurfaceGBufferType::eNormal)], ImGuiHelper::kGBufferDisplaySize);
//      ImGuiHelper::DrawRenderTexture("Depth", WaterSurfaceRenderTextures_[static_cast<uint>(WaterSurfaceGBufferType::eDepth)], ImGuiHelper::kGBufferDisplaySize);
//      //ImGuiHelper::DrawDepthTexture("Depth", DepthTexture_, ImGuiHelper::kGBufferDisplaySize);
//
//      ImGuiHelper::DrawRenderTexture("Reflection", WaterSurfaceReflectionRenderTexture_, ImGuiHelper::kGBufferDisplaySize);
//
//      ImGui::TreePop();
//    }
//    if (ImGui::TreeNode("ShadowPass"))
//    {
//      ImGuiHelper::DrawDepthTexture("ShadowMap", ShadowMap_, kHeightMapDisplaySize);
//
//      ImGui::TreePop();
//    }
//  }
//  ImGui::End();
//}
//
////--------------------------------------------------
//
//void SceneOcean::UpdateLightViewProjecion()
//{
//  Camera Light = Camera(DirectionalLightPosition_);
//  Light.isPerspective = false;
//  Light.Near = 5.0f;
//  Light.Far = 750.0f;
//
//  LightViewProjectionConstantBuffer_.GetData() = Light.View() * Light.Projection(Constants::kDefaultWindowSize / 4.0f);
//}
//
//void SceneOcean::DrawTerrain()const
//{
//  for (uint y = 0; y < kRetangleNum.y; ++y)
//  {
//    for (uint x = 0; x < kRetangleNum.x; ++x)
//    {
//      Renderer3D::Draw(Rectangle_, Matrix::Translate({ (x - kRetangleHalfNum.x) * kRetangleSize.x, 0.0f, (y - kRetangleHalfNum.y) * kRetangleSize.y }));
//    }
//  }
//}
