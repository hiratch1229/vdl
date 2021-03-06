#pragma once
#include "Scene.hpp"

class ScenePostEffect : public IScene
{
  static constexpr float kGroundScale = 18.0f;
  static constexpr vdl::uint2 kWindowSize = vdl::Constants::kDefaultWindowSize;
  static constexpr vdl::uint2 kGBufferSize = kWindowSize;
  static constexpr vdl::uint2 kShadowMapSize = vdl::uint2(4096, 4096);
  static constexpr vdl::uint kGBufferNum = 2; /* Diffuse + NormalMap */
  static constexpr vdl::uint kLightPassRenderTextureNum = 2;  /* Color + Luminance */
  static constexpr vdl::uint kShaderResourceNum = kGBufferNum + 2;  /* GBuffer + Depth + ShadowMap */
  static constexpr vdl::uint kShrinkBuffeNum = 4;
  static constexpr const char* kLigthPassPSFilePath = "Shader/PostEffect/LightPassPS.hlsl";
  static constexpr vdl::uint2 kSceneWindowSize = vdl::uint2(415, GUIHelper::kSceneWindowSize.y);
  static constexpr vdl::uint2 kShadowMapDisplaySize = vdl::uint2(GUIHelper::kGBufferDisplaySize.x);
private:
  struct RenderingData
  {
    vdl::float3 EyePos;
    float LuminanceThreshold;
    vdl::Color3F Fog;
    float Exposure;
    vdl::Color3F Shadow;
    float ShadowBias;
    vdl::Matrix InverseViewProjection;
  };
private:
  vdl::Model House_;
  vdl::StaticMesh Ground_;
  vdl::Camera Camera_;
  vdl::Matrix HouseWorld_;
  vdl::Matrix GroundWorld_;
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
  vdl::RenderTextures LightPassRenderTextures_;
  std::array<vdl::Texture, kShaderResourceNum> ShaderResources_;
  vdl::ConstantBuffer<vdl::DirectinalLight> DirectionalLightConstantBuffer_;
  vdl::ConstantBuffer<RenderingData> RenderingConstantBuffer_;
private:
  vdl::PixelShader VerticalGaussianBlurPixelShader_;
  vdl::PixelShader HorizontalGaussianBlurPixelShader_;
  vdl::PixelShader BloomPixelShader_;
  vdl::PixelShader TexturePixelShader_;
  vdl::RenderTextures ShrinkBuffers_;
public:
  ScenePostEffect() = default;

  void Initialize()override;

  void Update()override;
};
