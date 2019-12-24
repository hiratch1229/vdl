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
      UpdateData.PointLightRange = 1.0f;
    }
    Computer::SetConstantBuffers(0, 1, &UpdateConstantBuffer_);

    std::array<Data, kMaxDataNum> Datas;
    {
      for (auto& Data : Datas)
      {
        Data.Color = Color4F(Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f));
        Data.Timer = 0.0f;
        Data.Time = Random::Range(kMinUpdateTime, kMaxUpdateTime);

        switch (static_cast<PointLightMoveAxis>(rand() % kPointLightMoveAxisNum))
        {
        case PointLightMoveAxis::eMinusX:
        {
          Data.MinRange.x = Data.MaxRange.x = kPointLightMinMoveRange.x + Random::Range(0.0f, kPointLightAxisMargin.x);
          Data.MinRange.y = Random::Range(kPointLightMinMoveRange.y, kPointLightMaxMoveRange.y);
          Data.MaxRange.y = Random::Range(kPointLightMinMoveRange.y, kPointLightMaxMoveRange.y);
          Data.MinRange.z = Random::Range(kPointLightMinMoveRange.z, kPointLightMaxMoveRange.z);
          Data.MaxRange.z = Random::Range(kPointLightMinMoveRange.z, kPointLightMaxMoveRange.z);
        }
        break;
        case PointLightMoveAxis::eX:
        {
          Data.MinRange.x = Data.MaxRange.x = kPointLightMaxMoveRange.x - Random::Range(0.0f, kPointLightAxisMargin.x);
          Data.MinRange.y = Random::Range(kPointLightMinMoveRange.y, kPointLightMaxMoveRange.y);
          Data.MaxRange.y = Random::Range(kPointLightMinMoveRange.y, kPointLightMaxMoveRange.y);
          Data.MinRange.z = Random::Range(kPointLightMinMoveRange.z, kPointLightMaxMoveRange.z);
          Data.MaxRange.z = Random::Range(kPointLightMinMoveRange.z, kPointLightMaxMoveRange.z);
        }
        break;
        case PointLightMoveAxis::eY:
        {
          Data.MinRange.x = Random::Range(kPointLightMinMoveRange.x, kPointLightMaxMoveRange.x);
          Data.MaxRange.x = Random::Range(kPointLightMinMoveRange.x, kPointLightMaxMoveRange.x);
          Data.MinRange.y = Data.MaxRange.y = kPointLightMinMoveRange.y + Random::Range(0.0f, kPointLightAxisMargin.y);
          Data.MinRange.z = Random::Range(kPointLightMinMoveRange.z, kPointLightMaxMoveRange.z);
          Data.MaxRange.z = Random::Range(kPointLightMinMoveRange.z, kPointLightMaxMoveRange.z);
        }
        break;
        case PointLightMoveAxis::eMinusZ:
        {
          Data.MinRange.x = Random::Range(kPointLightMinMoveRange.x, kPointLightMaxMoveRange.x);
          Data.MaxRange.x = Random::Range(kPointLightMinMoveRange.x, kPointLightMaxMoveRange.x);
          Data.MinRange.y = Random::Range(kPointLightMinMoveRange.y, kPointLightMaxMoveRange.y);
          Data.MaxRange.y = Random::Range(kPointLightMinMoveRange.y, kPointLightMaxMoveRange.y);
          Data.MinRange.z = Data.MaxRange.z = kPointLightMinMoveRange.z + Random::Range(0.0f, kPointLightAxisMargin.z);
        }
        break;
        case PointLightMoveAxis::eZ:
        {
          Data.MinRange.x = Random::Range(kPointLightMinMoveRange.x, kPointLightMaxMoveRange.x);
          Data.MaxRange.x = Random::Range(kPointLightMinMoveRange.x, kPointLightMaxMoveRange.x);
          Data.MinRange.y = Random::Range(kPointLightMinMoveRange.y, kPointLightMaxMoveRange.y);
          Data.MaxRange.y = Random::Range(kPointLightMinMoveRange.y, kPointLightMaxMoveRange.y);
          Data.MinRange.z = Data.MaxRange.z = kPointLightMaxMoveRange.z - Random::Range(0.0f, kPointLightAxisMargin.z);
        }
        break;
        default: assert(false);
        }
      }
    }

    DatasUnorderedAccessBuffer_ = UnorderedAccessBuffer<Data>(kMaxDataNum, Datas.data());
    Computer::SetUnorderedAccessObjects(0, 1, &DatasUnorderedAccessBuffer_);
    PointLightsUnorderedAccessBuffer_ = UnorderedAccessBuffer<PointLight>(kMaxDataNum);
    Computer::SetUnorderedAccessObjects(1, 1, &PointLightsUnorderedAccessBuffer_);
  }

  //  GBufferPassの初期化
  {
    GBufferPassVertexShader_ = VertexShader("Shader/GBufferPass/GBufferPassVS.hlsl", InputLayoutType::eStaticMesh);
    GBufferPassPixelShader_ = PixelShader("Shader/TBDR/GBufferPassPS.hlsl");

    GBufferRenderTextures_[0] = RenderTexture(kWindowSize, FormatType::eR8G8B8A8_Unorm);
    GBufferRenderTextures_[1] = RenderTexture(kWindowSize, FormatType::eR8G8B8A8_Unorm);
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
      LightShaderResources_[kGBufferNum] = GBufferDepthTexture_.GetDepthTexture();
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
        RenderingData.AmbientColor = Palette::DimGray;
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
      Computer::SetConstantBuffers(1, 1, &DirectinalLightConstantBuffer_);
      Computer::SetConstantBuffers(2, 1, &RenderingConstantBuffer_);
      Computer::SetConstantBuffers(3, 1, &CameraConstantBuffer_);

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

    constexpr GUI::WindowFlags kSceneWindowBeginFlags = GUI::WindowFlags(GUI::WindowFlag::eNoMove) | GUI::WindowFlag::eNoResize;
    GUI::Begin("SceneTBDR", nullptr, kSceneWindowBeginFlags);
    {
      GUI::SetWindowPos(GUIHelper::kSceneWindowPos);
      GUI::SetWindowSize(kSceneWindowSize);
#if defined _DEBUG | DEBUG
      if (GUI::Button("Reload TileBaseCS"))
      {
        TileBaseComputeShader_ = ComputeShader(kTileBaseComputeShaderFilePath);
        Computer::SetShader(TileBaseComputeShader_);
      }
#endif
      GUI::Checkbox("TileBased", &isTileBase);
      uint CurrentPointLightNum = RenderingData.PointLightNum;
      if (GUI::Slider("PointLightNum", &CurrentPointLightNum, 0u, kMaxDataNum))
      {
        if (0 <= CurrentPointLightNum && CurrentPointLightNum <= kMaxDataNum)
        {
          RenderingData.PointLightNum = CurrentPointLightNum;
        }
      }
      GUI::ColorEdit("Ambient", &RenderingData.AmbientColor);
      if (GUI::TreeNode("DirectionalLight"))
      {
        DirectinalLight& DirectinalLight = DirectinalLightConstantBuffer_.GetData();
        GUI::Drag("Direction", &DirectinalLight.Direction);
        GUI::Text(std::string("Direction:" + std::to_string(DirectinalLight.Direction)).c_str());
        GUI::Drag("Itensity", &DirectinalLight.Itensity, 0.01f);
        GUI::ColorEdit("Color", &DirectinalLight.Color);
        GUI::TreePop();
      }
      if (GUI::TreeNode("PointLight"))
      {
        GUI::Drag("Itensity", &UpdateData.PointLightItensity, 0.01f);
        GUI::Drag("Range", &UpdateData.PointLightRange, 0.01f);
        GUI::TreePop();
      }
    }
    GUI::End();

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
    for (uint i = 0; i < kGBufferNum; ++i)
    {
      Renderer::Clear(GBufferRenderTextures_[i]);
    }
    Renderer::Clear(GBufferDepthTexture_);
    Renderer::Clear(LightUnorderedAccessTexture_);
  }

  //  GBufferPass
  {
    Renderer::SetRenderTextures(GBufferRenderTextures_, GBufferDepthTexture_);
    Renderer3D::SetStaticMeshShaders(GBufferPassVertexShader_, GBufferPassPixelShader_);

    Renderer3D::Draw(Sponza_, Matrix::Identity());
  }

  //  LightPass
  {
    Renderer::SetRenderTexture(Window::GetRenderTexture(), Window::GetDepthStencilTexture());

    if (isTileBase)
    {
      Computer::SetShaderResources(0, static_cast<uint>(LightShaderResources_.size() - 1), LightShaderResources_.data());

      Computer::Dispatch(kTileBaseDispatchNum);

      Renderer2D::Draw(LightUnorderedAccessTexture_);
    }
    else
    {
      Renderer::SetPixelStageShaderResources(0, static_cast<uint>(LightShaderResources_.size()), LightShaderResources_.data());

      Renderer::Draw(3);
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

      GUI::TreePop();
    }
  }
  GUI::End();
}
