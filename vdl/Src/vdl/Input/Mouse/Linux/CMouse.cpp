#include "CMouse.hpp"

#include <assert.h>

void CMouse::Initialize()
{

}

void CMouse::Update()
{

}

void CMouse::SetPos(const vdl::int2& _Pos)
{

}

void CMouse::Scroll(const vdl::int2& _Scroll)
{

}

void CMouse::SetCursor(MouseCursorType _Type)
{
    switch (_Type)
    {
    case MouseCursorType::eArrow:       break;
    case MouseCursorType::eTextInput:   break;
    case MouseCursorType::eResizeAll:   break;
    case MouseCursorType::eResizeEW:    break;
    case MouseCursorType::eResizeNS:    break;
    case MouseCursorType::eResizeNESW:  break;
    case MouseCursorType::eResizeNWSE:  break;
    case MouseCursorType::eHand:        break;
    case MouseCursorType::eNotArrow:    break;
    default: assert(false);
    }
  }
}
