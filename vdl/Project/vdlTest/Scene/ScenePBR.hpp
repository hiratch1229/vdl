#pragma once
#include "Scene.hpp"

class ScenePBR : public IScene
{
  static constexpr vdl::float3 kAnimationCubePosition = vdl::float3(-10.0f, 5.0f, 0.0f);
  static constexpr vdl::float3 kBoomBoxPosition = vdl::float3(-5.0f, 5.0f, 0.0f);
  static constexpr vdl::float3 kDamagedHelmetPosition = vdl::float3(0.0f, 5.0f, 0.0f);
  static constexpr vdl::float3 kSciFiHelmetPosition = vdl::float3(5.0f, 5.0f, 0.0f);
  static constexpr vdl::float3 kSuzannePosition = vdl::float3(10.0f, 5.0f, 0.0f);
  static constexpr vdl::float3 kTwoSidedPlanePosition = vdl::float3(0.0f, 0.0f, 0.0f);
  static constexpr vdl::float3 kAnimationCubeScale = vdl::float3(1.5f, 1.5f, 1.5f);
  static constexpr vdl::float3 kBoomBoxScale = vdl::float3(150.0f, 150.0f, 150.0f);
  static constexpr vdl::float3 kDamagedHelmetScale = vdl::float3(1.5f, 1.5f, 1.5f);
  static constexpr vdl::float3 kSciFiHelmetScale = vdl::float3(1.5f, 1.5f, 1.5f);
  static constexpr vdl::float3 kSuzanneScale = vdl::float3(1.5f, 1.5f, 1.5f);
  static constexpr vdl::float3 kTwoSidedPlaneScale = vdl::float3(15.0f, 15.0f, 15.0f);
private:
  struct Materials
  {
    vdl::Texture MetallicRoughness;
    vdl::Texture AmbientOcclusion;
    vdl::Texture Emissive;
  };
  struct MaterialData
  {
    vdl::float3 CameraPosition;
    float Reflectance;
    vdl::ColorF Ambient;
  };
private:
  vdl::Camera Camera_;
  vdl::VertexShader PBRVertexShader_;
  vdl::PixelShader PBRPixelShader_;
  vdl::ConstantBuffer<vdl::DirectinalLight> LightConstantData_;
  vdl::ConstantBuffer< MaterialData> MaterialDataConstantData_;
private:
  vdl::Model AnimatedCube_;
  Materials AnimatedMaterials_;
  vdl::Matrix AnimatedCubeWorld_;
  vdl::Model BoomBox_;
  Materials BoomBoxMaterials_;
  vdl::Matrix BoomBoxWorld_;
  vdl::Model DamagedHelmet_;
  Materials DamagedHelmetMaterials_;
  vdl::Matrix DamagedHelmetWorld_;
  vdl::Model SciFiHelmet_;
  Materials SciFiHelmetMaterials_;
  vdl::Matrix SciFiHelmetWorld_;
  vdl::Model Suzanne_;
  Materials SuzanneMaterials_;
  vdl::Matrix SuzanneWorld_;
  vdl::Model TwoSidedPlane_;
  Materials TwoSidedPlaneMaterials_;
  vdl::Matrix TwoSidedPlaneWorld_;
public:
  ScenePBR() = default;

  void Initialize()override;

  void Update()override;
};
