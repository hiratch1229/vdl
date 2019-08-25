#include "CWindow.hpp"

void CWindow::Initialize()
{
  constexpr DWORD kStyle = WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE;

  RECT Rect = { 0, 0, WindowSize_.x, WindowSize_.y };
  ::AdjustWindowRect(&Rect, kStyle, false);

  hWnd_ = CreateWindowA(Constants::kClassName,
    WindowName_,
    kStyle,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    Rect.right - Rect.left,
    Rect.bottom - Rect.top,
    ::GetDesktopWindow(),
    nullptr,
    GetModuleHandle(NULL),
    nullptr);
}

void CWindow::Show(bool _isShow)
{
  ::ShowWindow(hWnd_, _isShow);
}
