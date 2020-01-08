#pragma once
#include "Scene.hpp"

class SceneTessellation : public IScene
{
  struct ConstantBufferData
  {
    vdl::float3 TessFactor;
    float InsideFactor;
  };
private:
  static constexpr float kTessellationMinValue = 1.0f;
  static constexpr float kTessellationMaxValue = 64.0f;
private:
  vdl::StaticMesh Rectangle_;
  vdl::Camera Camera_;
  vdl::ConstantBuffer<ConstantBufferData> ConstantBuffer_;
public:
  SceneTessellation() = default;

  void Initialize()override;

  void Update()override;
};
