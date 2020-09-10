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
  vdl::Color4F ScreenClearColor = Constants::kDefaultScreenClearColor;
public:
  CWindow() = default;

  ~CWindow();

  PlatformFlags GetPlatform()const final { return PlatformFlag::eWindows; }

  void Initialize()override;

  void Update()override;

  void Show(bool _isShow)override;

  void* GetHandle()const override { return static_cast<void*>(hWnd_); }

  const vdl::uint2& GetWindowSize()const override { return WindowSize_; }

  const vdl::Color4F& GetScreenClearColor()const override { return ScreenClearColor; }
};
