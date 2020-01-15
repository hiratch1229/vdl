#pragma once
#include <vdl/Fwd.hpp>

#include <vdl/Platform/Platform.hpp>

class IWindow
{
public:
  static IWindow* Create();

  IWindow() = default;

  virtual ~IWindow() = default;

  [[nodiscard]] virtual PlatformFlags GetPlatform()const = 0;

  virtual void Initialize() = 0;

  virtual void Show(bool _isShow) = 0;

  [[nodiscard]] virtual const vdl::uint2& GetWindowSize()const = 0;

  [[nodiscard]] virtual const vdl::Color4F& GetScreenClearColor()const = 0;
};
