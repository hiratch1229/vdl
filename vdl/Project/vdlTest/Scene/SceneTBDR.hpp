#pragma once
#include "Scene.hpp"

class SceneTBDR : public IScene
{
  struct Range
  {
    float x;
    float z;
  };
  struct Data
  {
    vdl::ColorF Color;
    Range MinRange;
    Range MaxRange;
    float Timer;
    float Time;
    vdl::float2 Padding;
  };
private:
  static constexpr vdl::uint2 kWindowSize = vdl::Constants::kDefaultWindowSize;
  static constexpr float kRectangleScale = 15.0f;
  static constexpr float kRectangleHalfScale = kRectangleScale * 0.5f;
  static constexpr float kSphereScale = 0.5f;
  static constexpr vdl::uint kMaxDataNum = 1024;
  static constexpr vdl::uint kGBufferNum = 2;
  static constexpr vdl::uint2 kGBufferSize = kWindowSize;
  static constexpr vdl::uint2 kShadowMapSize = kWindowSize * 1;
  static constexpr vdl::uint2 kGBufferDisplaySize = kWindowSize / 5;
  static constexpr vdl::uint kGBufferLeftPos = kWindowSize.x - kGBufferDisplaySize.x;
  static constexpr vdl::uint kPointLightUpdateThreadNum = 1024;
  static constexpr vdl::uint3 kPointLightUpdateDispatchNum = vdl::uint3(kMaxDataNum / kPointLightUpdateThreadNum + (kMaxDataNum % kPointLightUpdateThreadNum == 0 ? 0 : 1), 1, 1);
  static constexpr float kMinUpdateTime = 5.0f;
  static constexpr float kMaxUpdateTime = 15.0f;
  static constexpr vdl::uint2 kTileSize = vdl::uint2(16, 16);
  static constexpr vdl::uint3 kTileBaseDispatchNum = vdl::uint3((kWindowSize.x + kTileSize.x - 1) / kTileSize.x, (kWindowSize.y + kTileSize.y - 1) / kTileSize.y, 1);
  static constexpr const char* kTileBaseComputeShaderFilePath = "Shader/TBDR/TileBaseCS.hlsl";
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
    vdl::float3 EyePosition;
    float SpecularPower;
    vdl::float3 AmbientColor;
    vdl::uint PointLightNum;
    vdl::ColorF Shadow;
    vdl::Matrix InverseViewProjection;
  };
  struct CameraData
  {
    vdl::Matrix View;
    vdl::Matrix Projection;
  };
private:
  vdl::Mesh Rectangle_;
  //vdl::Model Sponza_;
  vdl::Camera Camera_;
  bool isUpdate_ = true;
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
  vdl::VertexShader ShadowMapVertexShader_;
  vdl::PixelShader ShadowMapPixelShader_;
  vdl::DepthStencilTexture ShadowMap_;
  vdl::float3 DirectionLightPosition_;
  vdl::ConstantBuffer<vdl::Matrix> LightViewProjectionConstantBuffer_;
private:
  vdl::Sampler ShadowMapSampler_;
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

  ~SceneTBDR();

  void Initialize()override;

  void Update()override;

  SceneType GetType()const override { return SceneType::eTBDR; }
};
