#pragma once
#include "Scene.hpp"

class SceneMultiRenderTexture : public IScene
{
  static constexpr vdl::uint kUsingRenderTextureNum = 2;
  static constexpr vdl::uint2 kRenderTextureSize = vdl::Constants::kDefaultWindowSize;
  static constexpr vdl::Format kRenderTextureFormat = vdl::Constants::kSwapChainFormat;
private:
  vdl::Model Model_;
  vdl::RenderTextures RenderTextures_;
public:
  SceneMultiRenderTexture();

  ~SceneMultiRenderTexture();

  SceneType GetType()const override { return SceneType::eMultiRenderTexture; }

  void Update()override;
};
