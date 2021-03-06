#pragma once
#include "Scene.hpp"

#include <vector>

class SceneSprite : public IScene
{
  struct Data
  {
    vdl::float2 Pos;
    vdl::float2 Size;
    vdl::float2 SrcPos;
    vdl::float2 SrcSize;
    vdl::Degree Angle;
    vdl::Color4F Color = vdl::Palette::White;
  };
  using Datas = std::vector<Data>;
private:
  vdl::Texture Texture_;
  Datas Datas_;
public:
  SceneSprite() = default;

  void Initialize()override;

  void Update()override;
};
