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
  static constexpr vdl::uint2 kShadowMapSize = vdl::uint2(4096, 4096);
  static constexpr float kRectangleScale = 15.0f;
  static constexpr float kRectangleHalfScale = kRectangleScale * 0.5f;
  static constexpr float kSphereScale = 0.5f;
  static constexpr vdl::uint kDataNum = 128;
  static constexpr vdl::uint kGBufferNum = 3; /* Diffuse + NormalMap + Specular */
  static constexpr vdl::uint kShaderResourceNum = kGBufferNum + 2;  /* GBuffer + Depth + ShadowMap */
  static constexpr const char* kLigthPassPSFilePath = "Shader/Deferred/LightPassPS.hlsl";
  static constexpr vdl::uint2 kSceneWindowSize = vdl::uint2(300, ImGuiHelper::kSceneWindowSize.y);
  static constexpr vdl::uint2 kShadowMapDisplaySize = vdl::uint2(ImGuiHelper::kGBufferDisplaySize.x);
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
  vdl::StaticMesh Rectangle_;
  vdl::Texture RectangleSpecularMap_;
  vdl::StaticMesh Sphere_;
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
  std::array<vdl::Texture, kShaderResourceNum> ShaderResources_;
  vdl::ConstantBuffer<LightData> LightConstantBuffer_;
  vdl::ConstantBuffer<RenderingData> RenderingConstantBuffer_;
public:
  SceneDeferred() = default;

  void Initialize()override;

  void Update()override;
};
