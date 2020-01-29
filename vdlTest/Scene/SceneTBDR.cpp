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

  Camera_ = Camera(float3(6.0f, 5.0f, 0.0f), float3(-15.0f, 0.0f, 0.0f));
  FullScreenTriangleVertexShader_ = VertexShader("Shader/Option/FullScreenTriangleVS.hlsl", InputLayoutType::eNone);
  Renderer::SetVertexShader(FullScreenTriangleVertexShader_);

  //  更新用データの初期化
  {
    PointLightUpdateComputeShader_ = ComputeShader("Shader/TBDR/PointLightUpdateCS.hlsl");

    UpdateData& UpdateData = UpdateConstantBuffer_.GetData();
    {
      UpdateData.PointLightItensity = 0.5f;
      UpdateData.PointLightRange = 1.0f;
    }
    Computer::SetConstantBuffers(0, 1, &UpdateConstantBuffer_);

    std::array<PointLightData, kMaxPointLightNum> Datas;
    {
      enum class PointLightMoveAxis
      {
        eMinusX,
        eX,
        eY,
        eMinusZ,
        eZ,

        eNum
      };
      constexpr vdl::uint kPointLightMoveAxisNum = static_cast<vdl::uint>(PointLightMoveAxis::eNum);
      constexpr vdl::float3 kPointLightMinMoveRange = vdl::float3(-11.25f, 0.5f, -4.25f);
      constexpr vdl::float3 kPointLightMaxMoveRange = vdl::float3(10.1f, 10.5f, 5.0f);
      constexpr vdl::float3 kPointLightAxisMargin = vdl::float3(3.5f, 0.5f, 3.25f);

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

    PointLightDatasUnorderedAccessBuffer_ = UnorderedAccessBuffer<PointLightData>(kMaxPointLightNum, Datas.data());
    Computer::SetUnorderedAccessObjects(0, 1, &PointLightDatasUnorderedAccessBuffer_);
    PointLightsUnorderedAccessBuffer_ = UnorderedAccessBuffer<PointLight>(kMaxPointLightNum);
    Computer::SetUnorderedAccessObjects(1, 1, &PointLightsUnorderedAccessBuffer_);
  }

  //  GBufferPassの初期化
  {
    GBufferPassVertexShader_ = VertexShader("Shader/GBufferPass/GBufferPassVS.hlsl", InputLayoutType::eStaticMesh);
    GBufferPassPixelShader_ = PixelShader("Shader/TBDR/GBufferPassPS.hlsl");

    //  Diffuse
    GBufferRenderTextures_[0] = RenderTexture(kTextureSize, FormatType::eR8G8B8A8_Unorm);
    //  Normal
    GBufferRenderTextures_[1] = RenderTexture(kTextureSize, FormatType::eR8G8B8A8_Unorm);
    GBufferDepthTexture_ = DepthStencilTexture(kTextureSize, FormatType::eD32_Float);
  }

  //  LightPassの初期化
  {
    //  DeferredRendering
    {
      LightPassPixelShader_ = PixelShader("Shader/TBDR/LightPassPS.hlsl");
      LightRenderTexture_ = RenderTexture(kTextureSize, FormatType::eR16G16B16A16_Float);

      for (vdl::uint i = 0; i < kGBufferNum; ++i)
      {
        LightShaderResources_[i] = GBufferRenderTextures_[i];
      }
      LightShaderResources_[kGBufferNum] = GBufferDepthTexture_.GetDepthTexture();
      LightShaderResources_[kGBufferNum + 1] = PointLightsUnorderedAccessBuffer_.GetDetail();

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
        RenderingData.PointLightNum = kMaxPointLightNum;
      }
      Renderer::SetPixelStageConstantBuffers(1, 1, &RenderingConstantBuffer_);
      Renderer::SetTopology(TopologyType::eTriangleStrip);
    }

    //  Tile-based DeferredRendering
    {
      TileBaseComputeShader_ = ComputeShader(kTileBaseComputeShaderFilePath);
      LightUnorderedAccessTexture_ = UnorderedAccessTexture(kTextureSize, FormatType::eR16G16B16A16_Float);

      Computer::SetConstantBuffers(1, 1, &DirectinalLightConstantBuffer_);
      Computer::SetConstantBuffers(2, 1, &RenderingConstantBuffer_);
      Computer::SetConstantBuffers(3, 1, &CameraConstantBuffer_);
    }
  }

  //  Luminanceの初期化
  {
    LuminanceComputeShader_ = ComputeShader("Shader/TBDR/LuminanceCS.hlsl");
    LuminanceUnorderedAccessTexture_ = UnorderedAccessTexture(kTextureSize, FormatType::eR16G16B16A16_Float);
    Computer::SetUnorderedAccessObjects(3, 1, &LuminanceUnorderedAccessTexture_);

    BloomData& BloomData = BloomConstantBuffer_.GetData();
    {
      BloomData.LuminanceThreshold = 0.75f;
      BloomData.Exposure = 1.0f;
    }
    Computer::SetConstantBuffers(4, 1, &BloomConstantBuffer_);
  }

  //  PostProcessの初期化
  {
    VerticalGaussianBlurPixelShader_ = PixelShader("Shader/PostEffect/GaussianBlurPS.hlsl", "VertexBlur");
    HorizontalGaussianBlurPixelShader_ = PixelShader("Shader/PostEffect/GaussianBlurPS.hlsl", "HorizontalBlur");
    BloomPixelShader_ = PixelShader("Shader/PostEffect/BloomPS.hlsl");
    TexturePixelShader_ = PixelShader("Shader/Texture/TexturePS.hlsl");

    uint2 TextureSize = kTextureSize;
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
      if (GUI::Slider("PointLightNum", &CurrentPointLightNum, 0u, kMaxPointLightNum))
      {
        if (0 <= CurrentPointLightNum && CurrentPointLightNum <= kMaxPointLightNum)
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
      if (GUI::TreeNode("Bloom"))
      {
        BloomData& BloomData = BloomConstantBuffer_.GetData();
        GUI::Drag("LuminanceThreshold", &BloomData.LuminanceThreshold, 0.01f);
        GUI::Drag("Exposure", &BloomData.Exposure, 0.01f);
        GUI::TreePop();
      }
    }
    GUI::End();

    //  ポイントライトの更新
    {
      UpdateData.DeltaTime = System::GetDeltaTime();

      Computer::SetShader(PointLightUpdateComputeShader_);

      Computer::Dispatch(kPointLightUpdateDispatchNum);
    }

    if (isTileBase)
    {
      CameraData& CameraData = CameraConstantBuffer_.GetData();
      CameraData.View = Camera_.View();
      CameraData.Projection = Camera_.Projection(kTextureSize);
    }
  }

  //  画面のクリア
  {
    for (uint i = 0; i < kGBufferNum; ++i)
    {
      Renderer::Clear(GBufferRenderTextures_[i]);
    }
    Renderer::Clear(GBufferDepthTexture_);
    if (isTileBase)
    {
      Renderer::Clear(LightUnorderedAccessTexture_);
    }
    else
    {
      Renderer::Clear(LightRenderTexture_);
    }
    Renderer::Clear(LuminanceUnorderedAccessTexture_);
    for (uint i = 0; i < kShrinkBuffeNum; ++i)
    {
      Renderer::Clear(ShrinkBuffers_[i]);
    }
  }

  //  GBufferPass
  {
    Renderer::SetRenderTextures(GBufferRenderTextures_, GBufferDepthTexture_);
    Renderer3D::SetStaticMeshShaders(GBufferPassVertexShader_, GBufferPassPixelShader_);

    Renderer3D::Draw(Sponza_, Matrix::Identity());
  }

  //  LightPass
  {
    Renderer::SetRenderTexture(LightRenderTexture_, DepthStencilTexture());

    if (isTileBase)
    {
      Computer::SetShader(TileBaseComputeShader_);

      Computer::SetShaderResources(0, static_cast<uint>(LightShaderResources_.size() - 1), LightShaderResources_.data());

      Computer::SetUnorderedAccessObjects(2, 1, &LightUnorderedAccessTexture_);

      Computer::Dispatch(kTileBaseDispatchNum);

      UnorderedAccessTexture UnorderedAccessTexture;
      Computer::SetUnorderedAccessObjects(2, 1, &UnorderedAccessTexture);

      PostProcess(LightUnorderedAccessTexture_);
    }
    else
    {
      Renderer::SetPixelShader(LightPassPixelShader_);

      Renderer::SetPixelStageShaderResources(0, static_cast<uint>(LightShaderResources_.size()), LightShaderResources_.data());

      Renderer::Draw(3);

      PostProcess(LightRenderTexture_);
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
    if (GUI::TreeNode("LightPass"))
    {
      if (isTileBase)
      {
        GUIHelper::DrawUnorderedAccessTexture("Light", LightUnorderedAccessTexture_, GUIHelper::kGBufferDisplaySize);
      }
      else
      {
        GUIHelper::DrawRenderTexture("Light", LightRenderTexture_, GUIHelper::kGBufferDisplaySize);
      }
      GUIHelper::DrawUnorderedAccessTexture("Luminance", LuminanceUnorderedAccessTexture_, GUIHelper::kGBufferDisplaySize);

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

//---------------------------------------------

void SceneTBDR::PostProcess(const vdl::Texture& _Texture)
{
  //  Luminance
  {
    Computer::SetShader(LuminanceComputeShader_);

    Computer::SetShaderResources(3, 1, &_Texture);

    Computer::Dispatch(kLuminanceDispatchNum);

    Texture Texture;
    Computer::SetShaderResources(3, 1, &Texture);
  }

  //  PostProcess
  {
    std::array<vdl::ShaderResource, kShrinkBuffeNum> ShaderResources;
    Renderer2D::SetPixelStageShaderResources(1, static_cast<uint>(ShaderResources.size()), ShaderResources.data());

    //  Blur
    {
      Texture SrcTexture = LuminanceUnorderedAccessTexture_;
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

      Viewport.Size = kTextureSize;
      Renderer2D::SetViewport(Viewport);
    }

    //  Bloom
    {
      Renderer::SetRenderTexture(Window::GetRenderTexture(), Window::GetDepthStencilTexture());
      Renderer2D::SetPixelShader(BloomPixelShader_);
      Renderer2D::SetPixelStageShaderResources(1, kShrinkBuffeNum, ShrinkBuffers_.data());

      Renderer2D::Draw(_Texture);
    }
  }
}
