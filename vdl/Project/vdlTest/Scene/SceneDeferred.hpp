#pragma once
#include "Scene.hpp"

class SceneDeferred : public IScene
{
  vdl::Model Model_;
  vdl::Camera Camera_;
public:
  SceneDeferred() = default;

  ~SceneDeferred();

  void Initialize()override;

  void Update()override;

  SceneType GetType()const override { return SceneType::eDeferred; }
};
