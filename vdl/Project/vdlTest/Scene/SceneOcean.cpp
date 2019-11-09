#include "SceneOcean.hpp"

using namespace vdl;

void SceneOcean::Initialize()
{
  Camera_ = Camera(float3(0.0f, 25.0f, -60.0f));
  SwapchainRenderTexture_ = Window::GetRenderTexture();
  SwapchainDepthTexture_ = Window::GetDepthStencilTexture();

  //  êÖñ 
  {
    StaticModelData ModelData;
    ModelData.Vertices.resize(4);
    {
      ModelData.Vertices[0] = { { +50.0f, 0.0f, +50.0f }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
      ModelData.Vertices[1] = { { -50.0f, 0.0f, +50.0f }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
      ModelData.Vertices[2] = { { +50.0f, 0.0f, -50.0f }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };
      ModelData.Vertices[3] = { { -50.0f, 0.0f, -50.0f }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
    }
    ModelData.Indices = { 0, 1, 2, 3 };
    ModelData.MeshDatas.resize(1);
    {
      ModelData.MeshDatas[0].IndexCount = 4;
      ModelData.MeshDatas[0].Material.Diffuse = Palette::White;
    }

    WaterSurface_ = ModelData;

    WaterSurfaceVertexShader_ = VertexShader("Shader/Ocean/WaterSurfaceVS.hlsl", InputLayoutType::eStaticMesh);
    WaterSurfaceHullShader_ = HullShader("Shader/Ocean/WaterSurfaceHS.hlsl");
    WaterSurfaceDomainShader_ = DomainShader(kWaterSurfaceDomainShaderFilePath);
    WaterSurfacePixelShader_ = PixelShader("Shader/Ocean/WaterSurfacePS.hlsl");

    WaterSurfaceTessellationData& TessellationData = WaterSurfaceTessellationConstantBuffer_.GetData();
    {
      TessellationData.TessFactor = TessellationData.InsideFactor = 64.0f;
    }
    Renderer3D::SetHullStageConstantBuffers(0, 1, &WaterSurfaceTessellationConstantBuffer_);

    Renderer3D::SetDomainStageConstantBuffers(0, 1, &ViewProjectionConstantBuffer_);

    std::array<Wave, kMaxWaveNum>& Waves = WaterSurfaceWaveDatasConstantBuffer_.GetData();
    {
      for (auto& Wave : Waves)
      {
        const float Radian = Random::Range(0.0f, Math::ToRadian(360.0f)) * 0.3f;
        Wave.Direction = float3(sinf(Radian), 0.0f, cosf(Radian));
        Wave.Length = Random::Range(kWaveMinLength, kWaveMaxLength);
        //Wave.Speed = Random::Range(0.25f, 1.0f);
        Wave.Speed = 1.0f;
        //Wave.Steepness = Random::Range(0.75f, 1.0f);
        Wave.Steepness = 0.8f;
        Wave.Amplitude = 0.03f + powf(2.0f, Random::Range(0.0f, 2.0f)) * 0.05f;
      }
    }
    Renderer3D::SetDomainStageConstantBuffers(1, 1, &WaterSurfaceWaveDatasConstantBuffer_);

    WaterSurfaceGerstnerData& WaterSurfaceGerstnerData = WaterSurfaceGerstnerConstantBuffer_.GetData();
    {
      WaterSurfaceGerstnerData.Time = 0.0f;
      WaterSurfaceGerstnerData.WaveNum = kMaxWaveNum;
    }
    Renderer3D::SetDomainStageConstantBuffers(2, 1, &WaterSurfaceGerstnerConstantBuffer_);

    DirectinalLight& DirectionalLight = DirectionalLightConstantBuffer_.GetData();
    {
      DirectionalLight.Direction = Camera_.ViewVector().Normalize();
      DirectionalLight.Color = Palette::White;
      DirectionalLight.Itensity = 3.0f;
    }
    Renderer3D::SetPixelStageConstantBuffers(0, 1, &DirectionalLightConstantBuffer_);
  }
}

void SceneOcean::Update()
{
  //  çXêV
  {
    FreeCamera(&Camera_);
    ViewProjectionConstantBuffer_.GetData() = Camera_.View() * Camera_.Projection(Constants::kDefaultWindowSize);
    WaterSurfaceGerstnerConstantBuffer_.GetData().Time += System::GetDeltaTime();

    ImGui::Begin("SceneOcean");
    {
      if (ImGui::Checkbox("Wireframe", &isWireframe_))
      {
        Renderer3D::SetRasterizerState((isWireframe_ ? RasterizerState::kWireframeCullBack : RasterizerState::kSolidCullBack));
      }
      if (ImGui::TreeNode("DirectinalLight"))
      {
        DirectinalLight& DirectinalLight = DirectionalLightConstantBuffer_.GetData();
        ImGui::InputFloat3("Direction", &DirectinalLight.Direction.x);
        ImGui::InputFloat("Itensity", &DirectinalLight.Itensity);
        ImGui::ColorEdit3("Color", &DirectinalLight.Color.Red);
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("WaterSurface"))
      {
        if (ImGui::Button("Reload WaterSurfaceDS"))
        {
          WaterSurfaceDomainShader_ = DomainShader(kWaterSurfaceDomainShaderFilePath);
        }
        WaterSurfaceGerstnerData& WaterSurfaceGerstnerData = WaterSurfaceGerstnerConstantBuffer_.GetData();
        ImGui::SliderInt("WaveNum", reinterpret_cast<int*>(&WaterSurfaceGerstnerData.WaveNum), 1, kMaxWaveNum);
        if (ImGui::TreeNode("Tessellation"))
        {
          WaterSurfaceTessellationData& TessellationData = WaterSurfaceTessellationConstantBuffer_.GetData();
          ImGui::SliderFloat("TessFactor", &TessellationData.TessFactor, 1.0f, 64.0f);
          ImGui::SliderFloat("InsideFactor", &TessellationData.InsideFactor, 1.0f, 64.0f);
          ImGui::TreePop();
        }
        ImGui::TreePop();
      }
    }
    ImGui::End();
  }

  //  êÖÇÃï`âÊ
  {
    Renderer3D::SetStaticMeshShaders(WaterSurfaceVertexShader_, WaterSurfaceHullShader_, WaterSurfaceDomainShader_, WaterSurfacePixelShader_);
    Renderer3D::SetTopology(TopologyType::ePatchList4ControlPoint);

    Renderer3D::Draw(WaterSurface_, Matrix::Identity(), Palette::LightSkyBlue);
  }
}
