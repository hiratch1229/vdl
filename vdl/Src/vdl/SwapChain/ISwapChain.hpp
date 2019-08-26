#pragma once

class ISwapChain
{
public:
  ISwapChain() = default;

  virtual ~ISwapChain() = default;

  virtual void ScreenClear() = 0;

  virtual void Present() = 0;
};
