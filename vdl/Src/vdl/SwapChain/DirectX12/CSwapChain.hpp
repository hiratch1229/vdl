#pragma once
#include "../ISwapChain.hpp"

class CSwapChain : public ISwapChain
{
public:
  void Initialize()override;

  void ScreenClear()override;

  void Present()override;

  void ChangeWindowMode()override;

  void ScreenShot()override;
};
