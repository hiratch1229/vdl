#pragma once
#include "Scene.hpp"

class SceneOcean : public IScene
{
  static constexpr vdl::uint kGBufferNum = 2;
  static constexpr vdl::uint2 kGBufferSize = vdl::Constants::kDefaultWindowSize;
  static constexpr vdl::uint kMaxWaveNum = 25;
  static constexpr float kWaveMedianLength = 2.0f;
  static constexpr float kWaveMinLength = kWaveMedianLength * 0.5f;
  static constexpr float kWaveMaxLength = kWaveMedianLength * 2.0f;
  static constexpr const char* kWaterSurfaceDomainShaderFilePath = "Shader/Ocean/WaterSurfaceDS.hlsl";
private:
  struct Wave
  {
    vdl::float3 Direction;
    float Length;
    float Speed;
    float Steepness;
    float Amplitude;
    float Unused;
  };
private:
  struct WaterSurfaceTessellationData
  {
    float TessFactor;
    float InsideFactor;
    vdl::float2 Unused;
  };
  struct WaterSurfaceGerstnerData
  {
    float Time;
    vdl::uint WaveNum;
    vdl::float2 Unused;
  };
private:
  vdl::Mesh WaterSurface_;
  vdl::Camera Camera_;
  bool isWireframe_ = false;
private:
  vdl::VertexShader WaterSurfaceVertexShader_;
  vdl::HullShader WaterSurfaceHullShader_;
  vdl::DomainShader WaterSurfaceDomainShader_;
  vdl::PixelShader WaterSurfacePixelShader_;
  vdl::ConstantBuffer<WaterSurfaceTessellationData> WaterSurfaceTessellationConstantBuffer_;
  vdl::ConstantBuffer<vdl::Matrix> ViewProjectionConstantBuffer_;
  vdl::ConstantBuffer<std::array<Wave, kMaxWaveNum>> WaterSurfaceWaveDatasConstantBuffer_;
  vdl::ConstantBuffer<WaterSurfaceGerstnerData> WaterSurfaceGerstnerConstantBuffer_;
  vdl::ConstantBuffer<vdl::DirectinalLight> DirectionalLightConstantBuffer_;
private:
  vdl::RenderTexture SwapchainRenderTexture_;
  vdl::DepthStencilTexture SwapchainDepthTexture_;
public:
  SceneOcean() = default;

  void Initialize()override;

  void Update()override;
};
