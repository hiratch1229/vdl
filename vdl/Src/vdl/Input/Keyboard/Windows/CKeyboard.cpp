#include "CKeyboard.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>

#include <vdl/DetectMemoryLeak.hpp>

#include <vdl/pch/Windows/pch.hpp>

void CKeyboard::Update()
{
  if (!isInputAttach_)
  {
    ::AttachThreadInput(::GetWindowThreadProcessId(static_cast<HWND>(Engine::Get<IWindow>()->GetHandle()), nullptr), ::GetCurrentThreadId(), TRUE);
    isInputAttach_ = true;
  }

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
