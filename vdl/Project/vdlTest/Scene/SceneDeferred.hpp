#pragma once
#include "Scene.hpp"

#include <array>

class SceneDeferred : public IScene
{
  struct Range
  {
    float x;
    float z;
  };
  struct Data
  {
    vdl::float3 Position;
    vdl::ColorF Color;
    Range MinRange;
    Range MaxRange;
    float Timer;
    float Time;
  };
private:
  static constexpr vdl::uint2 kWindowSize = vdl::Constants::kDefaultWindowSize;
  static constexpr float kRectangleScale = 15.0f;
  static constexpr float kRectangleHalfScale = kRectangleScale * 0.5f;
  static constexpr float kSphereScale = 0.5f;
  static constexpr vdl::uint kDataNum = 128;
  static constexpr vdl::uint kUseRenderTextureNum = 3;
  static constexpr vdl::uint2 kGBufferDisplaySize = kWindowSize / 5;
  static constexpr vdl::uint kGBufferLeftPos = kWindowSize.x - kGBufferDisplaySize.x;
  static constexpr const char* kLigthPassPSFilePath = "Shader/Deferred/LightPassPS.hlsl";
private:
  struct LightData
  {
    vdl::DirectinalLight DirectionalLight;
    vdl::PointLights<kDataNum> PointLights;
  };
  struct RenderingData
  {
    vdl::float3 EyePosition;
    float SpecularPower;
    vdl::ColorF Ambient;
    vdl::ColorF Shadow;
    vdl::Matrix InverseViewProjection;
  };
private:
  vdl::Mesh Rectangle_;
  vdl::Texture RectangleSpecularMap_;
  vdl::Mesh Sphere_;
  vdl::Texture SphereSpecularMap_;
  vdl::Camera Camera_;
  float PointLightItensity_;
  float PointLightRange_;
  bool isUpdate_ = true;
  std::array<Data, kDataNum> Datas_;
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
  vdl::VertexShader LightPassVertexShader_;
  vdl::PixelShader LightPassPixelShader_;
  vdl::Sampler ShadowMapSampler_;
  std::array<vdl::Texture, kUseRenderTextureNum + 2> PixelStageShaderResources_;
  vdl::ConstantBuffer<LightData> LightConstantBuffer_;
  vdl::ConstantBuffer<RenderingData> RenderingConstantBuffer_;
public:
  SceneDeferred() = default;

  ~SceneDeferred();

  void Initialize()override;

  void Update()override;

  SceneType GetType()const override { return SceneType::eDeferred; }
};
