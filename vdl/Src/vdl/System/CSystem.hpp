#pragma once
#include "ISystem.hpp"

class CSystem : public ISystem
{
public:
  CSystem() = default;

  void Initialize()override;

  bool Update()override;

  float GetDeltaTime()override;
};
