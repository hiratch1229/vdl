#pragma once
#include "Scene.hpp"

class SceneOcean : public IScene
{
  static constexpr float kCameraNear = 5.0f;
  static constexpr float kCameraFar = 750.0f;
  static constexpr vdl::uint2 kRetangleNum = 10;
  static constexpr vdl::float2 kRetangleHalfNum = kRetangleNum / 2;
  static constexpr vdl::float2 kRetangleSize = 25.0f;
  static constexpr vdl::float2 kRetangleHalfSize = kRetangleSize / 2.0f;
  static constexpr vdl::float3 kSkyboxScale = 500.0f;
  static constexpr vdl::uint2 kGBufferSize = vdl::Constants::kDefaultWindowSize;
  static constexpr vdl::uint2 kShadowMapSize = vdl::uint2(4096, 4096);
  static constexpr vdl::uint2 kHeightMapSize = vdl::uint2(1024, 1024);
  static constexpr vdl::uint kTerrainTextureNum = 4;
  static constexpr const char* kTerrainTextureFileNames[kTerrainTextureNum] = { "sand_diffuse.png", "terrain_rock4.png", "terrain_slope.png","terrain_grass.png" };
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
  static constexpr const char* kWaterSurfaceReflectionPixelShaderFilePath = "Shader/Ocean/WaterSurface/ScreenSpaceReflectionPS.hlsl";
  static constexpr const char* kWaterSurfaceRefractionPixelShaderFilePath = "Shader/Ocean/WaterSurface/ScreenSpaceRefractionPS.hlsl";
  static constexpr vdl::uint kShrinkBuffeNum = 4;
  static constexpr vdl::uint kMaxRayMarchNum = 100;
  static constexpr vdl::uint2 kHeightMapDisplaySize = vdl::uint2(ImGuiHelper::kGBufferDisplaySize.x);
  static constexpr vdl::uint2 kSceneWindowSize = vdl::uint2(400, ImGuiHelper::kSceneWindowSize.y);
private:
  enum class GBufferType
  {
    eDiffuse,
    eNormal,
    eDepth,

    eNum
  };
  static constexpr vdl::uint kGBufferNum = static_cast<vdl::uint>(GBufferType::eNum);
  static constexpr const char* kGBufferNames[kGBufferNum] = { "Diffuse", "Normal", "Depth" };
  static constexpr vdl::ColorF kGBufferClearColors[kGBufferNum] = { vdl::ColorF(0.0f,0.0f,0.0f, 0.0f), vdl::ColorF(0.0f,0.0f,0.0f, 0.0f), vdl::ColorF(1.0f,0.0f,0.0f, 0.0f) };
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
  struct CameraData
  {
    vdl::Matrix ViewProjection;
    vdl::Matrix InverseViewProjection;
    vdl::float3 EyePostion;
    vdl::uint Unused;
    vdl::float3 ViewVector;
    vdl::uint Unused2;
  };
  struct LightData
  {
    vdl::Matrix ViewProjection;
    vdl::DirectinalLight DirectionalLight;
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
  struct WaterSurfaceData
  {
    float Time;
    vdl::uint WaveNum;
    vdl::float2 Unused;
  };
  struct ShadowData
  {
    vdl::ColorF Shadow; /* Color + Bias*/
  };
  struct RayMarchData
  {
    vdl::uint SampleNum;
    float MaxStep;
    float Step;
    float Unused;
  };
private:
  vdl::StaticMesh Rectangle_;
  vdl::StaticMesh Sphere_;
  vdl::Camera Camera_;
  vdl::RenderTexture SwapchainRenderTexture_;
  vdl::DepthStencilTexture DepthTexture_;
  vdl::RasterizerState RasterizerState_;
  vdl::ConstantBuffer<CameraData> CameraDataConstantBuffer_;
  vdl::ConstantBuffer<LightData> LightDataConstantBuffer_;
  bool isWireframe_ = false;
  bool isWaterSurfaceUpdate_ = true;
private:
  vdl::DepthStencilTexture ShadowMap_;
  vdl::float3 DirectionalLightPosition_;
  vdl::ConstantBuffer<ShadowData> ShadowDataConstantBuffer_;
private:
  vdl::RenderTextures GBufferTextures_;
  vdl::VertexShader StaticMeshVertexShader_;
  vdl::PixelShader StaticMeshPixelShader_;
  vdl::VertexShader FullScreenVertexShader_;
private:
  vdl::CubeTexture SkyboxTexture_;
  vdl::VertexShader SkyboxVertexShader_;
  vdl::PixelShader SkyboxPixelShader_;
private:
  vdl::RenderTexture TerrainTexcoordMap_;
  vdl::UnorderedAccessTexture TerrainHeightMap_;
  vdl::UnorderedAccessTexture TerrainNormalMap_;
  vdl::DepthStencilTexture TerrainTexcoordDepthTexture_;
  vdl::ComputeShader TerrainHeightMapUpdateComputeShader_;
  vdl::ComputeShader TerrainNormalMapUpdateComputeShader_;
  vdl::ConstantBuffer<TerrainUpdateData> TerrainUpdateDataConstantBuffer_;

  std::array<vdl::Texture, kTerrainTextureNum> TerrainTextures_;
  vdl::VertexShader TerrainVertexShader_;
  vdl::HullShader TerrainHullShader_;
  vdl::DomainShader TerrainDomainShader_;
  vdl::PixelShader TerrainPixelShader_;
  vdl::DomainShader TerrainTexcoordMapDomainShader_;
  vdl::DomainShader TerrainShadowDomainShader_;
  vdl::PixelShader TerrainTexcoordMapPixelShader_;
  vdl::ConstantBuffer<TerrainData> TerrainDataConstantBuffer_;
private:
  vdl::RenderTextures WaterSurfaceGBufferTextures_;
  std::array<vdl::Texture, kWaterSurfaceNormalMapNum> WaterSurfaceNormalMaps_;
  vdl::VertexShader WaterSurfaceVertexShader_;
  vdl::HullShader WaterSurfaceHullShader_;
  vdl::DomainShader WaterSurfaceDomainShader_;
  vdl::PixelShader WaterSurfacePixelShader_;
  vdl::ConstantBuffer<std::array<Wave, kMaxWaveNum>> WaterSurfaceWaveDatasConstantBuffer_;
  vdl::ConstantBuffer<WaterSurfaceData> WaterSurfaceDataConstantBuffer_;
  vdl::ConstantBuffer<RayMarchData> RayMarchConstantBuffer_;

  vdl::PixelShader WaterSurfaceRefractionPixelShader_;
  vdl::PixelShader WaterSurfaceReflectionPixelShader_;
//private:
//  vdl::RenderTextures OutputRenderTextures_;
//  vdl::RenderTextures ShrinkBuffers_;
//
//  vdl::PixelShader VerticalGaussianBlurPixelShader_;
//  vdl::PixelShader HorizontalGaussianBlurPixelShader_;
//  vdl::PixelShader BloomPixelShader_;
private:
  void UpdateLightViewProjecion();
  void DrawTerrain()const;
public:
  SceneOcean() = default;

  void Initialize()override;

  void Update()override;
};
