#pragma once
#include "Scene.hpp"

class SceneTBDR : public IScene
{
  static constexpr vdl::uint kMaxPointLightNum = 4096;
  static constexpr vdl::uint kPointLightUpdateThreadNum = 1024;
  static constexpr vdl::uint3 kPointLightUpdateDispatchNum = vdl::uint3(kMaxPointLightNum / kPointLightUpdateThreadNum + (kMaxPointLightNum % kPointLightUpdateThreadNum == 0 ? 0 : 1), 1, 1);
  static constexpr float kMinUpdateTime = 5.0f;
  static constexpr float kMaxUpdateTime = 15.0f;
  static constexpr vdl::uint2 kTextureSize = vdl::Constants::kDefaultWindowSize;
  static constexpr vdl::uint kGBufferNum = 2; /* Diffuse + NormalMap */
  static constexpr vdl::uint2 kTileSize = vdl::uint2(32, 32);
  static constexpr vdl::uint3 kTileBaseDispatchNum = vdl::uint3((kTextureSize.x + kTileSize.x - 1) / kTileSize.x, (kTextureSize.y + kTileSize.y - 1) / kTileSize.y, 1);
  static constexpr vdl::uint2 kLuminanceThreadGroupNum = vdl::uint2(32, 32);
  static constexpr vdl::uint3 kLuminanceDispatchNum = vdl::uint3(kTextureSize.x / kLuminanceThreadGroupNum.x + (kTextureSize.x % kLuminanceThreadGroupNum.x == 0 ? 0 : 1), kTextureSize.y / kLuminanceThreadGroupNum.y + (kTextureSize.y % kLuminanceThreadGroupNum.y == 0 ? 0 : 1), 1);
  static constexpr vdl::uint kShrinkBuffeNum = 4;
  static constexpr const char* kTileBaseComputeShaderFilePath = "Shader/TBDR/TileBaseCS.hlsl";
  static constexpr vdl::uint2 kSceneWindowSize = vdl::uint2(325, GUIHelper::kSceneWindowSize.y);
private:
  struct PointLightData
  {
    vdl::Color4F Color;
    vdl::float3 MinRange;
    float Timer;
    vdl::float3 MaxRange;
    float Time;
  };
  struct UpdateData
  {
    float DeltaTime;
    float PointLightItensity;
    float PointLightRange;
    float Unused;
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
  struct BloomData
  {
    float LuminanceThreshold;
    float Exposure;
    vdl::float2 Unused;
  };
private:
  void PostProcess(const vdl::Texture& _Texture);
private:
  vdl::Model Sponza_;
  vdl::Camera Camera_;
  bool isTileBase = true;
  vdl::VertexShader FullScreenTriangleVertexShader_;
private:
  vdl::ComputeShader PointLightUpdateComputeShader_;
  vdl::UnorderedAccessBuffer<PointLightData> PointLightDatasUnorderedAccessBuffer_;
  vdl::UnorderedAccessBuffer<vdl::PointLight> PointLightsUnorderedAccessBuffer_;
  vdl::ConstantBuffer<UpdateData> UpdateConstantBuffer_;
private:
  vdl::VertexShader GBufferPassVertexShader_;
  vdl::PixelShader GBufferPassPixelShader_;
  vdl::RenderTextures GBufferRenderTextures_;
  vdl::DepthStencilTexture GBufferDepthTexture_;
private:
  vdl::PixelShader LightPassPixelShader_;
  vdl::RenderTexture LightRenderTexture_;
  std::array<vdl::ShaderResource, kGBufferNum + 2> LightShaderResources_;
  vdl::ConstantBuffer<vdl::DirectinalLight> DirectinalLightConstantBuffer_;
  vdl::ConstantBuffer<RenderingData> RenderingConstantBuffer_;
private:
  vdl::ComputeShader TileBaseComputeShader_;
  vdl::UnorderedAccessTexture LightUnorderedAccessTexture_;
  vdl::ConstantBuffer<CameraData> CameraConstantBuffer_;
private:
  vdl::ComputeShader LuminanceComputeShader_;
  vdl::UnorderedAccessTexture LuminanceUnorderedAccessTexture_;
  vdl::ConstantBuffer<BloomData> BloomConstantBuffer_;
private:
  vdl::PixelShader VerticalGaussianBlurPixelShader_;
  vdl::PixelShader HorizontalGaussianBlurPixelShader_;
  vdl::PixelShader BloomPixelShader_;
  vdl::PixelShader TexturePixelShader_;
  vdl::RenderTextures ShrinkBuffers_;
public:
  SceneTBDR() = default;

  void Initialize()override;

  void Update()override;
};
