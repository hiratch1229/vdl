#pragma once
#include "Scene.hpp"

class SceneTBDR : public IScene
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
  struct Data
  {
    vdl::Color4F Color;
    vdl::float3 MinRange;
    float Timer;
    vdl::float3 MaxRange;
    float Time;
  };
private:
  static constexpr vdl::uint kPointLightMoveAxisNum = static_cast<vdl::uint>(PointLightMoveAxis::eNum);
  static constexpr vdl::uint2 kWindowSize = vdl::Constants::kDefaultWindowSize;
  static constexpr float kSphereScale = 0.5f;
  static constexpr vdl::float3 kPointLightMinMoveRange = vdl::float3(-11.25f, 0.5f, -4.25f);
  static constexpr vdl::float3 kPointLightMaxMoveRange = vdl::float3(10.1f, 10.5f, 5.0f);
  static constexpr vdl::float3 kPointLightAxisMargin = vdl::float3(3.5f, 0.5f, 3.25f);
  static constexpr vdl::uint kMaxDataNum = 4096;
  static constexpr vdl::uint kGBufferNum = 2; /* Diffuse + NormalMap */
  static constexpr vdl::uint kPointLightUpdateThreadNum = 1024;
  static constexpr vdl::uint3 kPointLightUpdateDispatchNum = vdl::uint3(kMaxDataNum / kPointLightUpdateThreadNum + (kMaxDataNum % kPointLightUpdateThreadNum == 0 ? 0 : 1), 1, 1);
  static constexpr float kMinUpdateTime = 5.0f;
  static constexpr float kMaxUpdateTime = 15.0f;
  static constexpr vdl::uint2 kTileSize = vdl::uint2(32, 32);
  static constexpr vdl::uint3 kTileBaseDispatchNum = vdl::uint3((kWindowSize.x + kTileSize.x - 1) / kTileSize.x, (kWindowSize.y + kTileSize.y - 1) / kTileSize.y, 1);
  static constexpr const char* kTileBaseComputeShaderFilePath = "Shader/TBDR/TileBaseCS.hlsl";
  static constexpr vdl::uint2 kSceneWindowSize = vdl::uint2(325, GUIHelper::kSceneWindowSize.y);
private:
  struct UpdateData
  {
    float DeltaTime;
    float PointLightItensity;
    float PointLightRange;
    float Padding;
  };
  struct RenderingData
  {
    vdl::Color3F AmbientColor;
    vdl::uint PointLightNum;
    vdl::Matrix InverseViewProjection;
  };
  struct CameraData
  {
    vdl::Matrix View;
    vdl::Matrix Projection;
  };
private:
  vdl::Model Sponza_;
  vdl::Camera Camera_;
  bool isTileBase = true;
private:
  vdl::UnorderedAccessBuffer<Data> DatasUnorderedAccessBuffer_;
  vdl::UnorderedAccessBuffer<vdl::PointLight> PointLightsUnorderedAccessBuffer_;
  vdl::ComputeShader PointLightUpdateComputeShader_;
  vdl::ConstantBuffer<UpdateData> UpdateConstantBuffer_;
private:
  vdl::VertexShader GBufferPassVertexShader_;
  vdl::PixelShader GBufferPassPixelShader_;
  vdl::RenderTextures GBufferRenderTextures_;
  vdl::DepthStencilTexture GBufferDepthTexture_;
private:
  std::array<vdl::ShaderResource, kGBufferNum + 3> LightShaderResources_;
  vdl::ConstantBuffer<vdl::DirectinalLight> DirectinalLightConstantBuffer_;
  vdl::ConstantBuffer<RenderingData> RenderingConstantBuffer_;
private:
  vdl::ComputeShader TileBaseComputeShader_;
  vdl::UnorderedAccessTexture LightUnorderedAccessTexture_;
private:
  vdl::VertexShader LightPassVertexShader_;
  vdl::PixelShader LightPassPixelShader_;
  vdl::ConstantBuffer<CameraData> CameraConstantBuffer_;
public:
  SceneTBDR() = default;

  void Initialize()override;

  void Update()override;
};
