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

  //  �J�[�\���̈ړ��ʂƍ��W��ۑ�
  {
    //  ���݂̃J�[�\���̈ʒu
    POINT Point;
    ::GetCursorPos(&Point);

    //  ���݂̃J�[�\���̈ʒu
    const vdl::int2 CursorPos = vdl::int2(Point.x / DpiScale_.x, Point.y / DpiScale_.y);

    //  �ړ��ʂ�ۑ�
    Delta_ = CursorPos - Pos_;

    //  ���W��ۑ�
    Pos_ = CursorPos;

    //  �z�C�[���̍X�V
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

  //  �J�[�\�����Z�b�g
  ::SetCursorPos(Pos_.x, Pos_.y);
}

void CMouse::Scroll(const vdl::int2& _Scroll)
{
  std::lock_guard Lock(ScrollMutex_);

  AccumulationScroll_ += _Scroll;
}
