#pragma once
#include "../IWindow.hpp"

#include <vdl/Type2.hpp>
#include <vdl/Constants/Constants.hpp>

#include <Windows.h>

class CWindow : public IWindow
{

private:
  HWND hWnd_;
  vdl::uint2 WindowSize_ = Constants::kDefaultWindowSize;
  const char* WindowName_ = Constants::kDefaultWindowName;
public:
  CWindow() = default;

  void Initialize()override;

  void Show(bool _isShow) override;

  void* GetHandle()const override { return hWnd_; }

  vdl::uint2 GetWindowSize()const override { return WindowSize_; }
};
