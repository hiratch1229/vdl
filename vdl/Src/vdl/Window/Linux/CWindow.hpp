#pragma once
#include "../IWindow.hpp"

#include <vdl/Type2.hpp>
#include <vdl/Color.hpp>
#include <vdl/Constants/Constants.hpp>

#include <vdl/pch/Linux/pch.hpp>

class CWindow : public IWindow
{
  xcb_connection_t* pConnect_;
  xcb_screen_t* pScreen_;
  vdl::uint Window_;
  xcb_intern_atom_reply_t* pDestoryReply_;
  vdl::uint2 WindowSize_ = Constants::kDefaultWindowSize;
  vdl::Color4F ScreenClearColor = Constants::kDefaultScreenClearColor;
public:
  CWindow() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eLinux; }

  void Initialize()override{}

  void Show(bool _isShow)override{}

  const vdl::uint2& GetWindowSize()const override { return WindowSize_; }

  const vdl::Color4F& GetScreenClearColor()const override { return ScreenClearColor; }

  xcb_connection_t* GetConnection()const { return pConnect_; }

  vdl::uint GetWindow()const { return Window_; }
};
