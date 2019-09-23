#pragma once
#include <vdl/Fwd.hpp>

class IWindow
{
public:
  static IWindow* Create();

  IWindow() = default;

  virtual ~IWindow() = default;

  virtual void Initialize() = 0;

  virtual void Show(bool _isShow) = 0;

  [[nodiscard]] virtual void* GetHandle()const = 0;

  [[nodiscard]] virtual const vdl::uint2& GetWindowSize()const = 0;

  [[nodiscard]] virtual const vdl::ColorF& GetScreenClearColor()const = 0;
};
