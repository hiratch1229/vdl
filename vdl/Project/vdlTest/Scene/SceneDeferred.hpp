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
  static constexpr vdl::uint kUseRenderTextureNum = 2;
private:
  vdl::Mesh Rectangle_;
  vdl::Mesh Sphere_;
  vdl::Camera Camera_;
  std::array<Data, kDataNum> Datas_;
private:
  vdl::VertexShader DeferredVertexShader_;
  vdl::PixelShader DeferredPixelShader_;
  vdl::RenderTextures DeferredRenderTextures_;
  vdl::DepthStencilTexture DeferredDepthTexture_;
public:
  SceneDeferred() = default;

  ~SceneDeferred();

  void Initialize()override;

  void Update()override;

  SceneType GetType()const override { return SceneType::eDeferred; }
};
