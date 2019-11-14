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

  //  �X�V�p�f�[�^�̏�����
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
        Data.Color = ColorF(Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f));
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

  //  GBufferPass�̏�����
  {
    GBufferPassVertexShader_ = VertexShader("Shader/GBufferPass/GBufferPassVS.hlsl", InputLayoutType::eStaticMesh);
    GBufferPassPixelShader_ = PixelShader("Shader/TBDR/GBufferPassPS.hlsl");

    GBufferRenderTextures_[0] = RenderTexture(kWindowSize, FormatType::eR8G8B8A8_Unorm);
    GBufferRenderTextures_[1] = RenderTexture(kWindowSize, FormatType::eR8G8B8A8_Unorm);
    GBufferDepthTexture_ = DepthStencilTexture(kWindowSize, FormatType::eD32_Float);
  }

  //  LightPass�̏�����
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
      Computer::SetConstantBuffers(1, 1, &DirectinalLightConstantBuffer_);
      Computer::SetConstantBuffers(2, 1, &RenderingConstantBuffer_);
      Computer::SetConstantBuffers(3, 1, &CameraConstantBuffer_);

      Computer::SetUnorderedAccessObjects(2, 1, &LightUnorderedAccessTexture_);
    }
  }
}

void SceneTBDR::Update()
{
  //  ���̍X�V
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

#if defined _DEBUG | DEBUG
      if (ImGui::Button("Reload TileBaseCS"))
      {
        TileBaseComputeShader_ = ComputeShader(kTileBaseComputeShaderFilePath);
        Computer::SetShader(TileBaseComputeShader_);
      }
#endif
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
      if (ImGui::TreeNode("GBuffer"))
      {
        for (vdl::uint i = 0; i < kGBufferNum; ++i)
        {
          if (ImGui::CollapsingHeader(kGBufferNames[i]))
          {
            ImGui::Image(GBufferRenderTextures_[i], kGBufferDisplaySize);
          }
        }
        ImGui::TreePop();
      }
    }
    ImGui::End();

    //  �|�C���g���C�g�̍X�V
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

  //  ��ʂ̃N���A
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
}
