#pragma once

class IRenderer
{
public:
  static IRenderer* Create();

  IRenderer() = default;

  virtual ~IRenderer() = default;
};
