#pragma once
#include <vdl.hpp>

class IScene
{
public:
  IScene() = default;

  virtual ~IScene() = default;

  virtual void Initialize() = 0;

  virtual void Update() = 0;
};
