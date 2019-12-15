#pragma once
#include "../IWindow.hpp"

#include <vdl/Type2.hpp>
#include <vdl/Color.hpp>
#include <vdl/Constants/Constants.hpp>

#include <vdl/pch/Windows/pch.hpp>

class CWindow : public IWindow
{
  HWND hWnd_;
  vdl::uint2 WindowSize_ = Constants::kDefaultWindowSize;
  const char* WindowName_ = Constants::kDefaultWindowName;
  vdl::ColorF ScreenClearColor = Constants::kDefaultScreenClearColor;
public:
  CWindow() = default;

  PlatformType GetPlatform()const final { return PlatformType::eWindows; }

  void Initialize()override;

  void Show(bool _isShow)override;

  void* GetHandle()const override { return hWnd_; }

  const vdl::uint2& GetWindowSize()const override { return WindowSize_; }

  const vdl::ColorF& GetScreenClearColor()const override { return ScreenClearColor; }
};
