#pragma once
#include "Scene.hpp"

class SceneGPUParticle : public IScene
{
  static constexpr vdl::uint kBufferNum = 100;
private:
  vdl::UnorderedAccessBuffer<vdl::int4> UnorderedAccessBuffer_;
public:
  SceneGPUParticle() = default;

  ~SceneGPUParticle();

  void Initialize()override;

  void Update()override;

  SceneType GetType()const override { return SceneType::eGPUParticle; }
};
