#pragma once
#include "Scene.hpp"

class SceneMultiRenderTexture : public IScene
{
  static constexpr vdl::uint kUsingRenderTextureNum = 2;
  static constexpr vdl::uint2 kRenderTextureSize = vdl::Constants::kDefaultWindowSize;
private:
  vdl::Model Model_;
  vdl::Camera Camera_;
  vdl::RenderTextures RenderTextures_;
public:
  SceneMultiRenderTexture() = default;

  void Initialize()override;

  void Update()override;

  SceneType GetType()const override { return SceneType::eMultiRenderTexture; }
};
