#include "SceneOcean.hpp"

using namespace vdl;

void SceneOcean::Initialize()
{
  StaticModelData ModelData;
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

  Rectangle_ = ModelData;

  Camera_ = Camera(float3(0.0f, 15.0f, -60.0f));
  SwapchainRenderTexture_ = Window::GetRenderTexture();
  SwapchainDepthTexture_ = Window::GetDepthStencilTexture();
  DirectionalLightPosition_ = Camera_.Position;

  LightData& LightData = LightDataConstantBuffer_.GetData();
  {
    LightData.Ambient = Palette::DimGray;
    LightData.DirectionalLight.Direction = -DirectionalLightPosition_.Normalize();
    LightData.DirectionalLight.Color = Palette::White;
    LightData.DirectionalLight.Itensity = 3.0f;
  }

  //  DeferredGBufferの初期化
  {
    DeferredGBufferTextures_[static_cast<uint>(GBufferType::eDiffuse)] = RenderTexture(kGBufferSize, FormatType::eR8G8B8A8_Unorm);
    DeferredGBufferTextures_[static_cast<uint>(GBufferType::eNormal)] = RenderTexture(kGBufferSize, FormatType::eR8G8B8A8_Unorm);

    Renderer3D::SetPixelStageConstantBuffers(0, 1, &LightDataConstantBuffer_);
  }

  //  ShadowMapの初期化
  {
    ShadowMap_ = DepthStencilTexture(kShadowMapSize, FormatType::eD32_Float);
  }

  //  LightPassの初期化
  {
    LightPassVertexShader_ = VertexShader("Shader/Option/FullScreenTriangleVS.hlsl", InputLayoutType::eNone);
    LightPassPixelShader_ = PixelShader("Shader/Ocean/LightPassPS.hlsl");

    LightPassData& LightPassData = LightPassDataConstantBuffer_.GetData();
    {
      LightPassData.Shadow = ColorF(Palette::DimGray, 0.0001f);
    }

    Renderer::SetPixelStageConstantBuffers(0, 1, &LightDataConstantBuffer_);
    Renderer::SetPixelStageConstantBuffers(1, 1, &LightPassDataConstantBuffer_);
    Renderer::SetTopology(TopologyType::eTriangleStrip);
  }

  //  空の初期化
  {
    Sphere_ = StaticModelData::Sphere(20, 20);
    SkyboxTexture_ = CubeTexture("Data/skybox.png");

    SkyboxVertexShader_ = VertexShader("Shader/Skybox/SkyboxVS.hlsl", InputLayoutType::eStaticMesh);
    SkyboxPixelShader_ = PixelShader("Shader/Skybox/SkyboxPS.hlsl");
  }

  //  地面の初期化
  {
    TerrainTexcoordMap_ = RenderTexture(kGBufferSize, FormatType::eR32G32_Float);
    TerrainHeightMap_ = UnorderedAccessTexture(kHeightMapSize, FormatType::eR32_Float);
    TerrainNormalMap_ = UnorderedAccessTexture(kHeightMapSize, FormatType::eR32G32B32A32_Float);
    TerrainTexcoordDepthTexture_ = DepthStencilTexture(kGBufferSize, FormatType::eD32_Float);
    TerrainHeightMapUpdateComputeShader_ = ComputeShader(kTerrainHeightMapUpdateComputeShaderFilePath);
    TerrainNormalMapUpdateComputeShader_ = ComputeShader(kTerrainNormalMapUpdateComputeShaderFilePath);
    Renderer::Clear(TerrainTexcoordMap_, kTerrainTexcoordMapClearColor);
    Renderer::Clear(TerrainNormalMap_, Palette::NormalMap);
    Renderer3D::SetDomainStageShaderResources(0, 1, &TerrainHeightMap_);
    Computer::SetShaderResources(0, 1, &TerrainTexcoordMap_);
    Computer::SetUnorderedAccessObjects(0, 1, &TerrainHeightMap_);
    Computer::SetUnorderedAccessObjects(1, 1, &TerrainNormalMap_);

    TerrainUpdateData& TerrainUpdateData = TerrainUpdateDataConstantBuffer_.GetData();
    {
      TerrainUpdateData.BlushSize = 15.0f;
      TerrainUpdateData.BlushHardness = 1.0f;
    }
    Computer::SetConstantBuffers(0, 1, &TerrainUpdateDataConstantBuffer_);

    TerrainVertexShader_ = VertexShader("Shader/Ocean/Terrain/TerrainVS.hlsl", InputLayoutType::eStaticMesh);
    TerrainHullShader_ = HullShader("Shader/Ocean/Terrain/TerrainHS.hlsl");
    TerrainDomainShader_ = DomainShader("Shader/Ocean/Terrain/TerrainDS.hlsl", "Color");
    TerrainPixelShader_ = PixelShader("Shader/Ocean/Terrain/TerrainPS.hlsl", "GBufferPass");
    TerrainShadowDomainShader_ = DomainShader("Shader/Ocean/Terrain/TerrainDS.hlsl", "Shadow");
    TerrainTexcoordMapDomainShader_ = DomainShader("Shader/Ocean/Terrain/TerrainDS.hlsl", "Texcoord");
    TerrainTexcoordMapPixelShader_ = PixelShader("Shader/Ocean/Terrain/TerrainPS.hlsl", "HeightMapUpdatePrePass");

    TessellationData& TessellationData = TessellationConstantBuffer_.GetData();
    {
      TessellationData.TessFactor = TessellationData.InsideFactor = 64.0f;
    }
    Renderer3D::SetHullStageConstantBuffers(0, 1, &TessellationConstantBuffer_);
  }

  //  水面の初期化
  {
    WaterSurfaceVertexShader_ = VertexShader("Shader/Ocean/WaterSurface/WaterSurfaceVS.hlsl", InputLayoutType::eStaticMesh);
    WaterSurfaceHullShader_ = HullShader("Shader/Ocean/WaterSurface/WaterSurfaceHS.hlsl");
    WaterSurfaceDomainShader_ = DomainShader(kWaterSurfaceDomainShaderFilePath);
    WaterSurfacePixelShader_ = PixelShader(kWaterSurfacePixelShaderFilePath);

    for (uint i = 0; i < kWaterSurfaceNormalMapNum; ++i)
    {
      char FIlePath[256];
      sprintf_s(FIlePath, "Data/Ocean/WaterNormalMap%d.png", i);
      WaterSurfaceNormalMaps_[i] = FIlePath;
    }

    Renderer3D::SetDomainStageConstantBuffers(0, 1, &ViewProjectionConstantBuffer_);

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
    Renderer3D::SetDomainStageConstantBuffers(1, 1, &WaterSurfaceWaveDatasConstantBuffer_);

    WaterSurfaceGerstnerData& WaterSurfaceGerstnerData = WaterSurfaceGerstnerConstantBuffer_.GetData();
    {
      WaterSurfaceGerstnerData.Time = 0.0f;
      WaterSurfaceGerstnerData.WaveNum = kMaxWaveNum;
    }
    Renderer3D::SetDomainStageConstantBuffers(2, 1, &WaterSurfaceGerstnerConstantBuffer_);
    Renderer3D::SetPixelStageConstantBuffers(1, 1, &WaterSurfaceGerstnerConstantBuffer_);

    WaterSurfaceRasterizerState_ = RasterizerState::kSolidCullNone;
  }
}

void SceneOcean::Update()
{
  //  更新
  {
    FreeCamera(&Camera_);
    Renderer3D::SetCamera(Camera_);

    Matrix& ViewProjection = ViewProjectionConstantBuffer_.GetData();
    {
      ViewProjection = Camera_.View() * Camera_.Projection(Constants::kDefaultWindowSize);
    }
    LightPassData& LightPassData = LightPassDataConstantBuffer_.GetData();
    {
      LightPassData.InverseViewProjection = ViewProjection.Inverse();
    }
    TerrainUpdateData& TerrainUpdateData = TerrainUpdateDataConstantBuffer_.GetData();

    ImGui::Begin("SceneOcean");
    {
      if (ImGui::Checkbox("Wireframe", &isWireframe_))
      {
        WaterSurfaceRasterizerState_ = (isWireframe_ ? RasterizerState::kWireframeCullNone : RasterizerState::kSolidCullNone);
      }
      if (ImGui::TreeNode("DirectinalLight"))
      {
        LightData& LightData = LightDataConstantBuffer_.GetData();
        if (ImGui::InputFloat3("Position", &DirectionalLightPosition_.x))
        {
          LightData.DirectionalLight.Direction = -DirectionalLightPosition_.Normalize();
        }
        ImGui::Text(std::string("Direction:" + std::to_string(LightData.DirectionalLight.Direction)).c_str());
        ImGui::InputFloat("Itensity", &LightData.DirectionalLight.Itensity);
        ImGui::ColorEdit3("Color", &LightData.DirectionalLight.Color);
        ImGui::ColorEdit3("Ambient", &LightData.Ambient);
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("Shadow"))
      {
        ImGui::ColorEdit3("Color", &LightPassData.Shadow);
        ImGui::InputFloat("Bias", &LightPassData.Shadow.Alpha);
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("Tessellation"))
      {
        TessellationData& TessellationData = TessellationConstantBuffer_.GetData();
        ImGui::SliderFloat("TessFactor", &TessellationData.TessFactor, 1.0f, 64.0f);
        ImGui::SliderFloat("InsideFactor", &TessellationData.InsideFactor, 1.0f, 64.0f);
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("Terrain"))
      {
#if DEBUG| _DEBUG
        if (ImGui::Button("Reload HeightMapUpdateCS"))
        {
          TerrainHeightMapUpdateComputeShader_ = ComputeShader(kTerrainHeightMapUpdateComputeShaderFilePath);
        }
#endif
        if (ImGui::Button("Clear HeightMap"))
        {
          Renderer::Clear(TerrainHeightMap_);
          Renderer::Clear(TerrainNormalMap_, Palette::NormalMap);
        }
        ImGui::SliderFloat("BlushSize", &TerrainUpdateData.BlushSize, 0.1f, 32.0f);
        ImGui::SliderFloat("BlushHardness", &TerrainUpdateData.BlushHardness, 0.0f, 1.0f);
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
#endif
        ImGui::Checkbox("isUpdate", &isWaterSurfaceUpdate_);
        WaterSurfaceGerstnerData& WaterSurfaceGerstnerData = WaterSurfaceGerstnerConstantBuffer_.GetData();
        ImGui::SliderInt("WaveNum", reinterpret_cast<int*>(&WaterSurfaceGerstnerData.WaveNum), 1, kMaxWaveNum);
        ImGui::TreePop();
      }
    }
    ImGui::End();

    if (isWaterSurfaceUpdate_)
    {
      WaterSurfaceGerstnerConstantBuffer_.GetData().Time += System::GetDeltaTime();
    }

    if (Input::isPress(Input::Mouse::ButtonLeft))
    {
      TerrainUpdateData.MousePosition = Input::Mouse::GetPos();

      //  ハイトマップの更新
      {
        Renderer::Clear(TerrainTexcoordMap_, kTerrainTexcoordMapClearColor);
        Renderer::Clear(TerrainTexcoordDepthTexture_);

        Renderer::SetRenderTexture(TerrainTexcoordMap_, TerrainTexcoordDepthTexture_);
        Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);
        Renderer3D::SetStaticMeshShaders(TerrainVertexShader_, TerrainHullShader_, TerrainTexcoordMapDomainShader_, TerrainTexcoordMapPixelShader_);
        Renderer3D::SetRasterizerState(RasterizerState::kSolidCullBack);

        DrawTerrain();

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

  //  画面のクリア
  {
    for (uint i = 0; i < kGBufferNum; ++i)
    {
      Renderer::Clear(DeferredGBufferTextures_[i]);
    }
    Renderer::Clear(ShadowMap_);
  }

  //  ShadowPass
  {
    Renderer::SetRenderTexture(RenderTexture(), ShadowMap_);
    Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);
    Renderer3D::SetStaticMeshShaders(TerrainVertexShader_, TerrainHullShader_, TerrainShadowDomainShader_, PixelShader());
    Renderer3D::SetRasterizerState(RasterizerState::kSolidCullBack);
  
    DrawTerrain();
  }

  //  GBufferPass
  {
    Renderer::SetRenderTextures(DeferredGBufferTextures_, SwapchainDepthTexture_);

    //  地面の描画
    {
      Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);
      Renderer3D::SetStaticMeshShaders(TerrainVertexShader_, TerrainHullShader_, TerrainDomainShader_, TerrainPixelShader_);
      Renderer3D::SetRasterizerState(WaterSurfaceRasterizerState_);
      Renderer3D::SetPixelStageShaderResources(2, 1, &TerrainNormalMap_);

      DrawTerrain();
    }
  }

  //  LightPass
  {
    Renderer::SetRenderTexture(SwapchainRenderTexture_, DepthStencilTexture());
    Renderer::SetShaders(LightPassVertexShader_, LightPassPixelShader_);
    Renderer::SetPixelStageShaderResources(0, kGBufferNum, DeferredGBufferTextures_.data());
    Renderer::SetPixelStageShaderResources(kGBufferNum, 1, &SwapchainDepthTexture_.GetDepthTexture());
    Renderer::SetPixelStageShaderResources(kGBufferNum + 1, 1, &ShadowMap_.GetDepthTexture());

    Renderer::Draw(3);
  }

  Renderer::SetRenderTexture(SwapchainRenderTexture_, SwapchainDepthTexture_);

  //  空の描画
  {
    Renderer3D::SetTopology(TopologyType::eDefaultMesh);
    Renderer3D::SetStaticMeshShaders(SkyboxVertexShader_, SkyboxPixelShader_);
    Renderer3D::SetPixelStageShaderResources(2, 1, &SkyboxTexture_);
    Renderer3D::SetRasterizerState(RasterizerState::kSolidCullFront);

    Renderer3D::Draw(Sphere_, Matrix::Scale(1000.0f));
  }

  //  水の描画
  {
    Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);
    Renderer3D::SetStaticMeshShaders(WaterSurfaceVertexShader_, WaterSurfaceHullShader_, WaterSurfaceDomainShader_, WaterSurfacePixelShader_);
    Renderer3D::SetRasterizerState(WaterSurfaceRasterizerState_);
    Renderer3D::SetPixelStageShaderResources(2, kWaterSurfaceNormalMapNum, WaterSurfaceNormalMaps_.data());

    for (uint y = 0; y < kRetangleNum.y; ++y)
    {
      for (uint x = 0; x < kRetangleNum.x; ++x)
      {
        Renderer3D::Draw(Rectangle_, Matrix::Translate({ (x - kRetangleHalfNum.x) * kRetangleSize.x, kWaterSurfaceHeight, (y - kRetangleHalfNum.y) * kRetangleSize.y }), ColorF(Palette::LightSkyBlue, 0.5f));
      }
    }
  }

  //  ShaderResourceの描画
  {
    ImGui::Begin("ShaderResources");
    {
      if (ImGui::CollapsingHeader("Terrain"))
      {
        ImGui::Image(TerrainTexcoordMap_, kGBufferDisplaySize);
        ImGui::Image(TerrainHeightMap_, kHeightMapDisplaySize);
        ImGui::Image(TerrainNormalMap_, kHeightMapDisplaySize);
      }
      if (ImGui::CollapsingHeader("ShadowMap"))
      {
        ImGui::Image(ShadowMap_.GetDepthTexture(), kGBufferDisplaySize);
      }
      if (ImGui::CollapsingHeader("GBuffer"))
      {
        for (uint i = 0; i < kGBufferNum; ++i)
        {
          ImGui::Image(DeferredGBufferTextures_[i], kGBufferDisplaySize);
        }
      }
    }
    ImGui::End();
  }
}

//--------------------------------------------------

void SceneOcean::DrawTerrain()
{
  for (uint y = 0; y < kRetangleNum.y; ++y)
  {
    for (uint x = 0; x < kRetangleNum.x; ++x)
    {
      Renderer3D::Draw(Rectangle_, Matrix::Translate({ (x - kRetangleHalfNum.x) * kRetangleSize.x, 0.0f, (y - kRetangleHalfNum.y) * kRetangleSize.y }));
    }
  }
}
