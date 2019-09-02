#include "CWindow.hpp"

#include <vdl/Misc/Windows/Misc.hpp>

void CWindow::Initialize()
{
  HRESULT hr = S_OK;

  //  COMÇÃèâä˙âª
  hr = ::CoInitialize(nullptr);
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));

  constexpr DWORD kStyle = WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE;

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

CWindow::~CWindow()
{
  //  COMÇÃâï˙
  ::CoUninitialize();
}

void CWindow::Show(bool _isShow)
{
  ::ShowWindow(hWnd_, _isShow);
}
