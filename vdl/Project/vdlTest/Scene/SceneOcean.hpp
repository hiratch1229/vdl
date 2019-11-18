#pragma once
#include "Scene.hpp"

class SceneOcean : public IScene
{
  static constexpr vdl::uint2 kRetangleNum = 10;
  static constexpr vdl::float2 kRetangleHalfNum = kRetangleNum / 2;
  static constexpr vdl::float2 kRetangleSize = 25.0f;
  static constexpr vdl::float2 kRetangleHalfSize = kRetangleSize / 2.0f;
  static constexpr vdl::uint2 kGBufferSize = vdl::Constants::kDefaultWindowSize;
  static constexpr vdl::uint2 kShadowMapSize = vdl::Constants::kDefaultWindowSize * 5;
  static constexpr vdl::uint2 kHeightMapSize = vdl::uint2(1024, 1024);
  static constexpr vdl::uint kTerrainTextureNum = 4;
  static constexpr const char* kTerrainHeightMapUpdateComputeShaderFilePath = "Shader/Ocean/Terrain/TerrainHeightMapUpdateCS.hlsl";
  static constexpr const char* kTerrainNormalMapUpdateComputeShaderFilePath = "Shader/Ocean/Terrain/TerrainNormalMapUpdateCS.hlsl";
  static constexpr const char* kTerrainPixelShaderFilePath = "Shader/Ocean/Terrain/TerrainPS.hlsl";
  static constexpr const char* kTerrainPixelShaderEntryPoint = "GBufferPass";
  static constexpr vdl::ColorF kTerrainTexcoordMapClearColor = vdl::ColorF(-1.0f, -1.0f, -1.0f, 0.0f);
  static constexpr vdl::uint2 kThreadGroupNum = vdl::uint2(32, 32);
  static constexpr vdl::uint3 kTerrainNormalMapDispatchNum = vdl::uint3(kHeightMapSize.x / kThreadGroupNum.x + (kHeightMapSize.x % kThreadGroupNum.x == 0 ? 0 : 1), kHeightMapSize.y / kThreadGroupNum.y + (kHeightMapSize.y % kThreadGroupNum.y == 0 ? 0 : 1), 1);
  static constexpr vdl::uint kMaxWaveNum = 25;
  static constexpr float kWaveMedianLength = 2.0f;
  static constexpr float kWaveMinLength = kWaveMedianLength * 0.5f;
  static constexpr float kWaveMaxLength = kWaveMedianLength * 2.0f;
  static constexpr float kWaterSurfaceHeight = 5.0f;
  static constexpr vdl::uint kWaterSurfaceNormalMapNum = 2;
  static constexpr const char* kWaterSurfaceDomainShaderFilePath = "Shader/Ocean/WaterSurface/WaterSurfaceDS.hlsl";
  static constexpr const char* kWaterSurfacePixelShaderFilePath = "Shader/Ocean/WaterSurface/WaterSurfacePS.hlsl";
  static constexpr vdl::uint2 kHeightMapDisplaySize = kHeightMapSize / 3;
  static constexpr vdl::uint2 kGBufferDisplaySize = kGBufferSize / 5;
private:
  enum class GBufferType
  {
    eDiffuse,
    eNormal,

    eNum
  };
  static constexpr vdl::uint kGBufferNum = static_cast<vdl::uint>(GBufferType::eNum);
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
  struct LightData
  {
    vdl::DirectinalLight DirectionalLight;
    vdl::ColorF Ambient;
  };
  struct LightPassData
  {
    vdl::ColorF Shadow; /* Color + Bias*/
    vdl::Matrix InverseViewProjection;
    vdl::Matrix LightViewProjection;
  };
  struct TessellationData
  {
    float TessFactor;
    float InsideFactor;
    vdl::float2 Unused;
  };
  struct TerrainUpdateData
  {
    vdl::int2 MousePosition;
    float BlushSize;
    float BlushHardness;
  };
  struct TerrainData
  {
    vdl::uint TextureLoopNum;
    float SandThreshold;
    float RockThreshold;
    float SlopeThreshold;
  };
  struct WaterSurfaceGerstnerData
  {
    float Time;
    vdl::uint WaveNum;
    vdl::float2 Unused;
  };
private:
  vdl::StaticMesh Rectangle_;
  vdl::StaticMesh Sphere_;
  vdl::CubeTexture SkyboxTexture_;
  vdl::Camera Camera_;
  vdl::float3 DirectionalLightPosition_;
  vdl::RenderTexture SwapchainRenderTexture_;
  vdl::DepthStencilTexture SwapchainDepthTexture_;
  bool isWireframe_ = false;
  bool isWaterSurfaceUpdate_ = true;
private:
  vdl::RenderTextures DeferredGBufferTextures_;
  vdl::VertexShader LightPassVertexShader_;
  vdl::PixelShader LightPassPixelShader_;
  vdl::ConstantBuffer<LightData> LightDataConstantBuffer_;
  vdl::ConstantBuffer<LightPassData> LightPassDataConstantBuffer_;
private:
  vdl::DepthStencilTexture ShadowMap_;
  vdl::VertexShader SkyboxVertexShader_;
  vdl::PixelShader SkyboxPixelShader_;
  vdl::ConstantBuffer<TessellationData> TessellationConstantBuffer_;
private:
  //vdl::UnorderedAccessTexture TerrainHeightMap_;
  //vdl::ComputeShader TerrainUpdateComputeShader_;

  std::array<vdl::Texture, kTerrainTextureNum> TerrainTextures_;
  vdl::RenderTexture TerrainTexcoordMap_;
  vdl::UnorderedAccessTexture TerrainHeightMap_;
  vdl::UnorderedAccessTexture TerrainNormalMap_;
  vdl::DepthStencilTexture TerrainTexcoordDepthTexture_;
  vdl::ComputeShader TerrainHeightMapUpdateComputeShader_;
  vdl::ComputeShader TerrainNormalMapUpdateComputeShader_;

  vdl::VertexShader TerrainVertexShader_;
  vdl::HullShader TerrainHullShader_;
  vdl::DomainShader TerrainDomainShader_;
  vdl::PixelShader TerrainPixelShader_;
  vdl::DomainShader TerrainShadowDomainShader_;
  vdl::DomainShader TerrainTexcoordMapDomainShader_;
  vdl::PixelShader TerrainTexcoordMapPixelShader_;
  vdl::ConstantBuffer<TerrainUpdateData> TerrainUpdateDataConstantBuffer_;
  vdl::ConstantBuffer<TerrainData> TerrainDataConstantBuffer_;
private:
  vdl::VertexShader WaterSurfaceVertexShader_;
  vdl::HullShader WaterSurfaceHullShader_;
  vdl::DomainShader WaterSurfaceDomainShader_;
  vdl::PixelShader WaterSurfacePixelShader_;
  std::array<vdl::Texture, kWaterSurfaceNormalMapNum> WaterSurfaceNormalMaps_;
  vdl::RasterizerState WaterSurfaceRasterizerState_;
  vdl::ConstantBuffer<vdl::Matrix> ViewProjectionConstantBuffer_;
  vdl::ConstantBuffer<std::array<Wave, kMaxWaveNum>> WaterSurfaceWaveDatasConstantBuffer_;
  vdl::ConstantBuffer<WaterSurfaceGerstnerData> WaterSurfaceGerstnerConstantBuffer_;
private:
  void DrawTerrain();
public:
  SceneOcean() = default;

  void Initialize()override;

  void Update()override;
};
