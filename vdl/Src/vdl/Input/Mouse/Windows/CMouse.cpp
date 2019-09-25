#include "CMouse.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>
#include <vdl/Input/Keyboard/IKeyboard.hpp>

void CMouse::Initialize()
{
  pKeyboard_ = Engine::Get<IKeyboard>();
  hWnd_ = static_cast<HWND>(Engine::Get<IWindow>()->GetHandle());

  HDC DisplayDeviceContext = ::GetDC(::GetDesktopWindow());
  DpiScale_ = { ::GetDeviceCaps(DisplayDeviceContext , LOGPIXELSX), ::GetDeviceCaps(DisplayDeviceContext , LOGPIXELSY) };
  DpiScale_ /= 96.0f;
}

void CMouse::Update()
{
  constexpr vdl::uint kVirtualCodes[kButtonNum] = { VK_LBUTTON, VK_MBUTTON, VK_RBUTTON, VK_XBUTTON1, VK_XBUTTON2 };

  for (vdl::uint i = 0; i < kButtonNum; ++i)
  {
    InputStatus_[i].Update(pKeyboard_->Press(kVirtualCodes[i]));
  }

  //  カーソルの移動量と座標を保存
  {
    //  現在のカーソルの位置
    POINT Point;
    ::GetCursorPos(&Point);
    ::ScreenToClient(hWnd_, &Point);

    //  現在のカーソルの位置
    const vdl::int2 CursorPos = vdl::int2(Point.x / DpiScale_.x, Point.y / DpiScale_.y);

    //  移動量を保存
    Delta_ = CursorPos - Pos_;

    //  座標を保存
    Pos_ = CursorPos;

    //  ホイールの更新
    {
      std::lock_guard Lock(ScrollMutex_);

      Scroll_ = AccumulationScroll_;

      AccumulationScroll_ = 0;
    }
  }
}

void CMouse::SetPos(const vdl::int2& _Pos)
{
  Pos_ = _Pos;

  //  カーソルをセット
  ::SetCursorPos(Pos_.x, Pos_.y);
}

void CMouse::Scroll(const vdl::int2& _Scroll)
{
  std::lock_guard Lock(ScrollMutex_);

  AccumulationScroll_ += _Scroll;
}
