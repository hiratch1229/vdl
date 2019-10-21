#pragma once
#include "Scene.hpp"

class ScenePostEffect : public IScene
{
  vdl::Model House_;
  vdl::Mesh Ground_;
  vdl::Camera Camera_;
public:
  ScenePostEffect() = default;

  ~ScenePostEffect();

  void Initialize()override;

  void Update()override;

  SceneType GetType()const override { return SceneType::ePostEffect; }
};
