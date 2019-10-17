#pragma once
#include "Scene.hpp"

#include <array>

class SceneDeferred : public IScene
{
  struct Range
  {
    float x;
    float z;
  };
  struct Data
  {
    vdl::ColorF Color;
    Range MinRange;
    Range MaxRange;
    float Timer;
    float Time;
  };
private:
  static constexpr float kRectangleScale = 15.0f;
  static constexpr float kRectangleHalfScale = kRectangleScale*0.5f;
  static constexpr vdl::uint kDataNum = 100;
private:
  vdl::Mesh Rectangle_;
  vdl::Mesh Sphere_;
  vdl::Camera Camera_;
  std::array<Data, kDataNum> Datas_;
public:
  SceneDeferred() = default;

  ~SceneDeferred();

  void Initialize()override;

  void Update()override;

  SceneType GetType()const override { return SceneType::eDeferred; }
};
