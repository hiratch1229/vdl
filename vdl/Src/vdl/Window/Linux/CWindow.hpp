#pragma once
#include "../IWindow.hpp"

#include <vdl/Type2.hpp>
#include <vdl/Color.hpp>
#include <vdl/Constants/Constants.hpp>

class CWindow : public IWindow
{
  vdl::uint2 WindowSize_ = Constants::kDefaultWindowSize;
  vdl::Color4F ScreenClearColor = Constants::kDefaultScreenClearColor;
public:
  CWindow() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eLinux; }

  void Initialize()override{}

  void Show(bool _isShow)override{}

  void* GetHandle()const override { return nullptr; }

  const vdl::uint2& GetWindowSize()const override { return WindowSize_; }

  const vdl::Color4F& GetScreenClearColor()const override { return ScreenClearColor; }
};
