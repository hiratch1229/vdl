#pragma once
#include <vdl.hpp>

enum class SceneType : vdl::uint
{
  eMultiRenderTexture,

  eNum
};

class IScene
{
public:
  IScene() = default;

  virtual ~IScene() = default;

  [[nodiscard]] virtual SceneType GetType()const = 0;

  virtual void Update() = 0;
};
