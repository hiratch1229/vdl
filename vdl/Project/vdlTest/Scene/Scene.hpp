#pragma once
#include <vdl.hpp>

enum class SceneType : vdl::uint
{
  eSprite,
  eModel,
  eMultiRenderTexture,
  eTessellation,
  eGPUParticle,

  eDeferred,

  eNum
};

class IScene
{
public:
  IScene() = default;

  virtual ~IScene() = default;

  virtual void Initialize() = 0;

  virtual void Update() = 0;

  [[nodiscard]] virtual SceneType GetType()const = 0;
};
