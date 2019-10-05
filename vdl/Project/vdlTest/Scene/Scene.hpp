#pragma once
#include <vdl.hpp>

enum class SceneType : vdl::uint
{
  eMultiRenderTexture,
  eTessellation,
  eGPUParticle,

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
