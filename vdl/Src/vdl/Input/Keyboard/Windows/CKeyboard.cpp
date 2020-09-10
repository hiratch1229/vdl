#include "CKeyboard.hpp"

#include <vdl/DetectMemoryLeak.hpp>

#include <vdl/pch/Windows/pch.hpp>

void CKeyboard::Update()
{
  BYTE KeyState[kKeyNum];
  //  �S�ẴL�[�̏����擾
  if (::GetKeyboardState(KeyState))
  {
    //  �S�ẴL�[�̏����X�V
    for (vdl::uint i = 0; i < kKeyNum; ++i)
    {
      InputStatus_[i].Update((KeyState[i] & 0x80) != 0);
    }
  }
}
