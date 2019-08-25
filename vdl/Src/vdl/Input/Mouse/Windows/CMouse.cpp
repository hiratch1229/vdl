#include "CMouse.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Input/Keyboard/IKeyboard.hpp>

#include <Windows.h>

void CMouse::Initialize()
{
  pKeyboard_ = Engine::Get<IKeyboard>();
}

void CMouse::Update()
{
  static constexpr int kVirtualCodes[kButtonNum] = { VK_LBUTTON, VK_MBUTTON, VK_RBUTTON, VK_XBUTTON1, VK_XBUTTON2 };

  for (int i = 0; i < kButtonNum; ++i)
  {
    InputStatus_[i].Update(pKeyboard_->Press(kVirtualCodes[i]));
  }

  //  カーソルの移動量と座標を保存
  {
    //  現在のカーソルの位置
    POINT Point;
    ::GetCursorPos(&Point);

    //  現在のカーソルの位置
    const vdl::int2 CursorPos = vdl::int2(Point.x / DpiScale_.x, Point.y / DpiScale_.y);

    //  移動量を保存
    Delta_ = CursorPos - Pos_;

    //  座標を保存
    Pos_ = CursorPos;

    Wheel_ = 0;
  }
}

void CMouse::SetPos(vdl::int2 _Pos)
{
  Pos_ = _Pos;

  //  ウィンドウの中心にカーソルをセット
  ::SetCursorPos(Pos_.x, Pos_.y);
}
