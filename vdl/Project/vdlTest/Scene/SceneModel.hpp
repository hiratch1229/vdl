#pragma once
#include "Scene.hpp"

#include <vector>

class SceneModel : public IScene
{
  vdl::Model Model_;
  vdl::Camera Camera_;
  vdl::uint DrawNum_ = 1;
public:
  SceneModel() = default;

  ~SceneModel();

  void Initialize();

  void Update()override;

  SceneType GetType()const override { return SceneType::eModel; }
};
