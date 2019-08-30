#pragma once

class ISystem
{
public:
  static ISystem* Create();

  ISystem() = default;

  virtual ~ISystem() = default;

  virtual void Initialize() = 0;

  virtual bool Update() = 0;

  virtual float GetDeltaTime() = 0;
};
