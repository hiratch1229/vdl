#include "SceneTBDR.hpp"

using namespace vdl;

namespace
{
  float QuadraticFunction(float _Time, float _TotalTime, float _Min, float _Max)
  {
    return (1.0f - (_Time / (_TotalTime / 2.0f) - 1.0f) * (_Time / (_TotalTime / 2.0f) - 1.0f)) * (_Max - _Min) + _Min;
  }
}

void SceneTBDR::Initialize()
{
  Rectangle_ = MeshData::Rectangle("Data/asphalt/asphalt.jpg", "Data/asphalt/asphalt_normal.jpg");
  //Sponza_ = Model("Data/sponza.fbx");

  Camera_ = Camera(float3(0.0f, 5.0f, -15.0f));

  //  更新用データの初期化
  {
    PointLightUpdateComputeShader_ = ComputeShader("Shader/TBDR/PointLightUpdateCS.hlsl");

    UpdateData& UpdateData = UpdateConstantBuffer_.GetData();
    {
      UpdateData.PointLightItensity = 0.5f;
      UpdateData.PointLightRange = 0.75f;
    }
    Computer::SetConstantBuffers(0, 1, &UpdateConstantBuffer_);

    std::array<Data, kMaxDataNum> Datas;
    {
      for (auto& Data : Datas)
      {
        Data.Color = ColorF(Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f));
        Data.MinRange.x = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
        Data.MinRange.z = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
        Data.MaxRange.x = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
        Data.MaxRange.z = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
        Data.Timer = 0.0f;
        Data.Time = Random::Range(kMinUpdateTime, kMaxUpdateTime);
      }
    }

    DatasUnorderedAccessBuffer_ = UnorderedAccessBuffer<Data>(kMaxDataNum, Datas.data());
    Computer::SetUnorderedAccessObjects(0, 1, &DatasUnorderedAccessBuffer_);
    PointLightsUnorderedAccessBuffer_ = UnorderedAccessBuffer<PointLight>(kMaxDataNum);
    Computer::SetUnorderedAccessObjects(1, 1, &PointLightsUnorderedAccessBuffer_);
  }

  //  GBufferPassの初期化
  {
    GBufferPassVertexShader_ = VertexShader("Shader/TBDR/GBufferPassVS.hlsl", InputLayoutType::eMesh);
    GBufferPassPixelShader_ = PixelShader("Shader/TBDR/GBufferPassPS.hlsl");

    GBufferRenderTextures_[0] = RenderTexture(kWindowSize, FormatType::eR8G8B8A8_Unorm);
    GBufferRenderTextures_[1] = RenderTexture(kWindowSize, FormatType::eR16G16B16A16_Float);
    GBufferDepthTexture_ = DepthStencilTexture(kWindowSize, FormatType::eD32_Float);
  }

  //  ShadowMapの初期化
  {
    ShadowMapVertexShader_ = VertexShader("Shader/TBDR/ShadowMapVS.hlsl", InputLayoutType::eMesh);
    ShadowMapPixelShader_ = PixelShader();
    ShadowMap_ = DepthStencilTexture(kWindowSize, FormatType::eD32_Float);

    DirectionLightPosition_ = Camera_.Position;

    Renderer3D::SetVertexStageConstantBuffers(1, 1, &LightViewProjectionConstantBuffer_);
  }

  //  LightPassの初期化
  {
    //  DeferredRendering
    {
      LightPassVertexShader_ = VertexShader("Shader/TBDR/LightPassVS.hlsl", InputLayoutType::eNone);
      LightPassPixelShader_ = PixelShader("Shader/TBDR/LightPassPS.hlsl");
      ShadowMapSampler_ = Sampler(AddressModeType::eBorder, AddressModeType::eBorder, AddressModeType::eBorder,
        FilterType::eAnisotropic, 16, BorderColorType::eWhite);
      Renderer::SetPixelStageSamplers(0, 1, &ShadowMapSampler_);

      for (vdl::uint i = 0; i < kGBufferNum; ++i)
      {
        LightShaderResources_[i] = GBufferRenderTextures_[i];
      }
      LightShaderResources_[kGBufferNum] = GBufferDepthTexture_;
      LightShaderResources_[kGBufferNum + 1] = ShadowMap_;
      LightShaderResources_[kGBufferNum + 2] = PointLightsUnorderedAccessBuffer_.GetDetail();

      Renderer::SetShaders(LightPassVertexShader_, LightPassPixelShader_);
      DirectinalLight& DirectinalLight = DirectinalLightConstantBuffer_.GetData();
      {
        DirectinalLight.Direction = float3(float3(0.0f) - DirectionLightPosition_).Normalize();
        DirectinalLight.Itensity = 0.125f;
        DirectinalLight.Color = Palette::White;
      }
      Renderer::SetPixelStageConstantBuffers(0, 1, &DirectinalLightConstantBuffer_);
      RenderingData& RenderingData = RenderingConstantBuffer_.GetData();
      {
        RenderingData.SpecularPower = 15.0f;
        RenderingData.AmbientColor = { 0.5f,0.5f,0.5f };
        RenderingData.PointLightNum = kMaxDataNum;
        RenderingData.Shadow = ColorF(Palette::DimGray, 0.0001f);
      }
      Renderer::SetPixelStageConstantBuffers(1, 1, &RenderingConstantBuffer_);
      Renderer::SetPixelStageConstantBuffers(2, 1, &LightViewProjectionConstantBuffer_);
      Renderer::SetTopology(TopologyType::eTriangleStrip);
    }

    //  Tile-based DeferredRendering
    {
      TileBaseComputeShader_ = ComputeShader(kTileBaseComputeShaderFilePath);
      LightUnorderedAccessTexture_ = UnorderedAccessTexture(kWindowSize, FormatType::eR16G16B16A16_Float);

      Computer::SetSamplers(0, 1, &ShadowMapSampler_);

      Computer::SetShader(TileBaseComputeShader_);
      Computer::SetConstantBuffers(1, 1, &RenderingConstantBuffer_);
      Computer::SetConstantBuffers(2, 1, &LightViewProjectionConstantBuffer_);
      Computer::SetConstantBuffers(3, 1, &CameraConstantBuffer_);
      Computer::SetConstantBuffers(4, 1, &DirectinalLightConstantBuffer_);

      Computer::SetUnorderedAccessObjects(2, 1, &LightUnorderedAccessTexture_);
    }
  }
}

SceneTBDR::~SceneTBDR()
{
  Renderer3D::SetShaders(VertexShader("Shader/Mesh/MeshVS.hlsl", InputLayoutType::eMesh),
    PixelShader("Shader/Mesh/MeshPS.hlsl"));
  Renderer::SetTopology(TopologyType::eDefaultNone);
}

void SceneTBDR::Update()
{
  //  情報の更新
  {
    FreeCamera(&Camera_);
    Renderer3D::SetCamera(Camera_);

    RenderingData& RenderingData = RenderingConstantBuffer_.GetData();
    {
      RenderingData.EyePosition = Camera_.Position;
      RenderingData.InverseViewProjection = (Renderer3D::GetView() * Renderer3D::GetProjection()).Inverse();
    }
    UpdateData& UpdateData = UpdateConstantBuffer_.GetData();

    ImGui::Begin("SceneTBDR");
    {
      DirectinalLight& DirectinalLight = DirectinalLightConstantBuffer_.GetData();

      if (ImGui::Button("Reload TileBaseCS"))
      {
        TileBaseComputeShader_ = ComputeShader(kTileBaseComputeShaderFilePath);
        Computer::SetShader(TileBaseComputeShader_);
      }
      ImGui::Checkbox("TileBased", &isTileBase);
      ImGui::Checkbox("SphereUpdate", &isUpdate_);
      int CurrentPointLightNum = static_cast<int>(RenderingData.PointLightNum);
      if (ImGui::SliderInt("PointLightNum", &CurrentPointLightNum, 0, kMaxDataNum))
      {
        if (0 <= CurrentPointLightNum && CurrentPointLightNum <= kMaxDataNum)
        {
          RenderingData.PointLightNum = CurrentPointLightNum;
        }
      }
      ImGui::InputFloat("SpecularPower", &RenderingData.SpecularPower);
      ImGui::ColorEdit3("Ambient", &RenderingData.AmbientColor.x);
      ImGui::ColorEdit3("Shadow", &RenderingData.Shadow.Red);
      ImGui::InputFloat("ShadowBias", &RenderingData.Shadow.Alpha);
      if (ImGui::TreeNode("DirectionalLight"))
      {
        if (ImGui::InputFloat3("Position", &DirectionLightPosition_.x))
        {
          DirectinalLight.Direction = float3(float3(0.0f) - DirectionLightPosition_).Normalize();
        }
        ImGui::Text(std::string("Direction:" + std::to_string(DirectinalLight.Direction)).c_str());
        ImGui::InputFloat("Itensity", &DirectinalLight.Itensity);
        ImGui::ColorEdit3("Color", &DirectinalLight.Color.Red);
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("PointLight"))
      {
        ImGui::InputFloat("Itensity", &UpdateData.PointLightItensity);
        ImGui::InputFloat("Range", &UpdateData.PointLightRange);
        ImGui::TreePop();
      }
    }
    ImGui::End();

    const Camera Light = Camera(DirectionLightPosition_, float3(0.0f), float3::Up());
    LightViewProjectionConstantBuffer_.GetData() = Light.View() * Light.Projection(kWindowSize);

    if (isTileBase)
    {
      CameraData& CameraData = CameraConstantBuffer_.GetData();
      CameraData.View = Camera_.View();
      CameraData.Projection = Camera_.Projection(kWindowSize);
    }
    if (isUpdate_)
    {
      UpdateData.DeltaTime = System::GetDeltaTime();

      Computer::SetShader(PointLightUpdateComputeShader_);

      Computer::Dispatch(kPointLightUpdateDispatchNum);

      Computer::SetShader(TileBaseComputeShader_);
    }
  }

  //  画面のクリア
  {
    for (vdl::uint i = 0; i < kGBufferNum; ++i)
    {
      Renderer::Clear(GBufferRenderTextures_[i]);
    }
    Renderer::Clear(GBufferDepthTexture_);
    Renderer::Clear(ShadowMap_);
    Renderer::Clear(LightUnorderedAccessTexture_);
  }
  const vdl::uint RenderingNum = RenderingConstantBuffer_.GetData().PointLightNum;

  const Matrix RectangleWorld = Matrix::Scale(kRectangleScale) * Matrix::Rotate(Math::ToRadian(90.0f), 0.0f, 0.0f);
  //  GBufferPass
  {
    Renderer::SetRenderTextures(GBufferRenderTextures_, GBufferDepthTexture_);
    Renderer3D::SetShaders(GBufferPassVertexShader_, GBufferPassPixelShader_);
    Renderer3D::Draw(Rectangle_, RectangleWorld);

    //Renderer3D::Draw(Sponza_, Matrix::Identity());
  }

  //  ShadowPass
  {
    Renderer::SetRenderTexture(RenderTexture(), ShadowMap_);
    Renderer3D::SetShaders(ShadowMapVertexShader_, ShadowMapPixelShader_);

    //Renderer3D::Draw(Sponza_, Matrix::Identity());
  }

  //  LightPass
  {
    Renderer::SetRenderTexture(RenderTexture(), DepthStencilTexture());

    if (isTileBase)
    {
      Computer::SetShaderResources(0, static_cast<vdl::uint>(LightShaderResources_.size() - 1), LightShaderResources_.data());

      Computer::Dispatch(kTileBaseDispatchNum);

      decltype(LightShaderResources_) ShaderResources;
      Computer::SetShaderResources(0, static_cast<vdl::uint>(ShaderResources.size() - 1), ShaderResources.data());

      Renderer2D::Draw(LightUnorderedAccessTexture_);
    }
    else
    {
      Renderer::SetPixelStageShaderResources(0, static_cast<vdl::uint>(LightShaderResources_.size()), LightShaderResources_.data());

      Renderer::Draw(4);

      decltype(LightShaderResources_) ShaderResources;
      Renderer2D::SetPixelStageShaderResources(2, static_cast<vdl::uint>(ShaderResources.size()) - 2, ShaderResources.data());
    }
  }

  ////  GBufferの描画
  //for (vdl::uint i = 0; i < LightShaderResources_.size(); ++i)
  //{
  //  Renderer2D::Draw(LightShaderResources_[i], float2(kGBufferLeftPos, kGBufferDisplaySize.y * i), kGBufferDisplaySize);
  //}
}
