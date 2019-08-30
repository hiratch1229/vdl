#include "CKeyboard.hpp"

#include <Windows.h>

void CKeyboard::Initialize()
{

}

void CKeyboard::Update()
{
  BYTE KeyState[kKeyNum];
  //  �S�ẴL�[�̏����擾
  if (::GetKeyboardState(KeyState))
  {
    //  �S�ẴL�[�̏����X�V
    for (int i = 0; i < kKeyNum; ++i)
    {
      InputStatus_[i].Update((KeyState[i] & 0x80) != 0);
    }
  }
}
