#pragma once

class IGUI
{
public:
  static IGUI* Create();

  IGUI() = default;

  virtual ~IGUI() = default;

  virtual void Initialize() = 0;

  virtual void Update() = 0;

  virtual void Draw() = 0;
};
