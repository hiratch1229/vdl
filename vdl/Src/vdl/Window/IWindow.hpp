#pragma once
#include <vdl/Type2.hpp>

class IWindow
{
public:
  static IWindow* Create();

  IWindow() = default;

  virtual ~IWindow() = default;

  virtual void Initialize() = 0;

  virtual void Show(bool _isShow) = 0;

  virtual void* GetHandle()const = 0;

  virtual vdl::uint2 GetWindowSize()const = 0;
};
