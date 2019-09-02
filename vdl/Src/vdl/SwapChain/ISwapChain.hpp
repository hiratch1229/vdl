#pragma once

class ISwapChain
{
public:
  static ISwapChain* Create();

  ISwapChain() = default;

  virtual ~ISwapChain() = default;
  
  virtual void Initialize() = 0;

  virtual void ScreenClear() = 0;

  virtual void Present() = 0;

  virtual void ChangeWindowMode() = 0;

  virtual void ScreenShot() = 0;
};
