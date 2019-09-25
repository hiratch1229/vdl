#include "CWindow.hpp"

#include <vdl/Misc/Windows/Misc.hpp>

void CWindow::Initialize()
{
  HRESULT hr = S_OK;

  constexpr DWORD kStyle = WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME;

  RECT Rect = { 0, 0, static_cast<long>(WindowSize_.x), static_cast<long>(WindowSize_.y) };
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
