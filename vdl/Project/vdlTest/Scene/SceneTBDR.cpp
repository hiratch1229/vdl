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
  Sponza_ = Model("Data/sponza/Sponza.gltf");

  Camera_ = Camera(float3(6.0f, 3.0f, 0.0f));

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
        Data.MinRange = float3(Random::Range(kMinRange.x, kMaxRange.x), Random::Range(kMinRange.y, kMaxRange.y), Random::Range(kMinRange.z, kMaxRange.z));
        Data.MaxRange = float3(Random::Range(kMinRange.x, kMaxRange.x), Random::Range(kMinRange.y, kMaxRange.y), Random::Range(kMinRange.z, kMaxRange.z));
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
    GBufferPassVertexShader_ = VertexShader("Shader/GBufferPass/GBufferPassVS.hlsl", InputLayoutType::eMesh);
    GBufferPassPixelShader_ = PixelShader("Shader/TBDR/GBufferPassPS.hlsl");

    GBufferRenderTextures_[0] = RenderTexture(kWindowSize, FormatType::eR8G8B8A8_Unorm);
    GBufferRenderTextures_[1] = RenderTexture(kWindowSize, FormatType::eR8G8B8A8_Snorm);
    GBufferDepthTexture_ = DepthStencilTexture(kWindowSize, FormatType::eD32_Float);
  }

  //  LightPassの初期化
  {
    //  DeferredRendering
    {
      LightPassVertexShader_ = VertexShader("Shader/Option/FullScreenTriangleVS.hlsl", InputLayoutType::eNone);
      LightPassPixelShader_ = PixelShader("Shader/TBDR/LightPassPS.hlsl");

      for (vdl::uint i = 0; i < kGBufferNum; ++i)
      {
        LightShaderResources_[i] = GBufferRenderTextures_[i];
      }
      LightShaderResources_[kGBufferNum] = GBufferDepthTexture_;
      LightShaderResources_[kGBufferNum + 2] = PointLightsUnorderedAccessBuffer_.GetDetail();

      Renderer::SetShaders(LightPassVertexShader_, LightPassPixelShader_);
      DirectinalLight& DirectinalLight = DirectinalLightConstantBuffer_.GetData();
      {
        DirectinalLight.Direction = float3(0.0f, 0.0f, 1.0f).Normalize();
        DirectinalLight.Itensity = 0.125f;
        DirectinalLight.Color = Palette::White;
      }
      Renderer::SetPixelStageConstantBuffers(0, 1, &DirectinalLightConstantBuffer_);
      RenderingData& RenderingData = RenderingConstantBuffer_.GetData();
      {
        RenderingData.AmbientColor = { 0.5f,0.5f,0.5f };
        RenderingData.PointLightNum = kMaxDataNum;
      }
      Renderer::SetPixelStageConstantBuffers(1, 1, &RenderingConstantBuffer_);
      Renderer::SetTopology(TopologyType::eTriangleStrip);
    }

    //  Tile-based DeferredRendering
    {
      TileBaseComputeShader_ = ComputeShader(kTileBaseComputeShaderFilePath);
      LightUnorderedAccessTexture_ = UnorderedAccessTexture(kWindowSize, FormatType::eR16G16B16A16_Float);

      Computer::SetShader(TileBaseComputeShader_);
      Computer::SetConstantBuffers(1, 1, &RenderingConstantBuffer_);
      Computer::SetConstantBuffers(3, 1, &CameraConstantBuffer_);
      Computer::SetConstantBuffers(4, 1, &DirectinalLightConstantBuffer_);

      Computer::SetUnorderedAccessObjects(2, 1, &LightUnorderedAccessTexture_);
    }
  }
}

void SceneTBDR::Update()
{
  //  情報の更新
  {
    FreeCamera(&Camera_);
    Renderer3D::SetCamera(Camera_);

    RenderingData& RenderingData = RenderingConstantBuffer_.GetData();
    {
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
      int CurrentPointLightNum = static_cast<int>(RenderingData.PointLightNum);
      if (ImGui::SliderInt("PointLightNum", &CurrentPointLightNum, 0, kMaxDataNum))
      {
        if (0 <= CurrentPointLightNum && CurrentPointLightNum <= kMaxDataNum)
        {
          RenderingData.PointLightNum = CurrentPointLightNum;
        }
      }
      ImGui::ColorEdit3("Ambient", &RenderingData.AmbientColor.x);
      if (ImGui::TreeNode("DirectionalLight"))
      {
        ImGui::InputFloat3("Direction", &DirectinalLight.Direction);
        ImGui::Text(std::string("Direction:" + std::to_string(DirectinalLight.Direction)).c_str());
        ImGui::InputFloat("Itensity", &DirectinalLight.Itensity);
        ImGui::ColorEdit3("Color", &DirectinalLight.Color);
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

    //  ポイントライトの更新
    {
      UpdateData.DeltaTime = System::GetDeltaTime();

      Computer::SetShader(PointLightUpdateComputeShader_);

      Computer::Dispatch(kPointLightUpdateDispatchNum);

      Computer::SetShader(TileBaseComputeShader_);
    }

    if (isTileBase)
    {
      CameraData& CameraData = CameraConstantBuffer_.GetData();
      CameraData.View = Camera_.View();
      CameraData.Projection = Camera_.Projection(kWindowSize);
    }
  }

  //  画面のクリア
  {
    for (vdl::uint i = 0; i < kGBufferNum; ++i)
    {
      Renderer::Clear(GBufferRenderTextures_[i]);
    }
    Renderer::Clear(GBufferDepthTexture_);
    Renderer::Clear(LightUnorderedAccessTexture_);
  }
  const vdl::uint RenderingNum = RenderingConstantBuffer_.GetData().PointLightNum;

  //  GBufferPass
  {
    Renderer::SetRenderTextures(GBufferRenderTextures_, GBufferDepthTexture_);
    Renderer3D::SetShaders(GBufferPassVertexShader_, GBufferPassPixelShader_);

    Renderer3D::Draw(Sponza_, Matrix::Identity());
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

      Renderer::Draw(3);

      decltype(LightShaderResources_) ShaderResources;
      Renderer2D::SetPixelStageShaderResources(2, static_cast<vdl::uint>(ShaderResources.size()) - 2, ShaderResources.data());
    }
  }
}
