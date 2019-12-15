#pragma once
#include <vdl/Platform/Platform.hpp>

class IGUI
{
public:
  static IGUI* Create();

  IGUI() = default;

  virtual ~IGUI() = default;

  [[nodiscard]] virtual PlatformType GetPlatform()const = 0;

  virtual void Initialize() = 0;

  virtual void Update() = 0;

  virtual void Draw() = 0;
};
