#include "CWindow.hpp"

#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/DetectMemoryLeak.hpp>

void CWindow::Initialize()
{
  HRESULT hr = S_OK;

  constexpr DWORD kStyle = WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME;

  RECT Rect = { 0, 0, static_cast<long>(WindowSize_.x), static_cast<long>(WindowSize_.y) };
  ::AdjustWindowRect(&Rect, kStyle, false);

  wchar_t wWindowName[Constants::kMaxCharacterNum];
  ::mbsrtowcs_s(nullptr, wWindowName, &WindowName_, Constants::kMaxCharacterNum, nullptr);
  WindowName_;

  hWnd_ = ::CreateWindow(Constants::kClassName,
    wWindowName,
    kStyle,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    Rect.right - Rect.left,
    Rect.bottom - Rect.top,
    nullptr,
    nullptr,
    ::GetModuleHandle(nullptr),
    nullptr);
}

void CWindow::Update()
{
  MSG Msg = {};
  //  メッセージを全て処理
  while (::PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
  {
    ::TranslateMessage(&Msg);
    ::DispatchMessage(&Msg);
  }
}

CWindow::~CWindow()
{
  ::DestroyWindow(hWnd_);
}

void CWindow::Show(bool _isShow)
{
  ::ShowWindow(hWnd_, _isShow);
  ::SetForegroundWindow(hWnd_);
}
