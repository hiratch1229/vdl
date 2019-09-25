#include "CKeyboard.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Window/IWindow.hpp>

#include <Windows.h>

void CKeyboard::Initialize()
{

}

void CKeyboard::Update()
{
  if (!isInputAttach_)
  {
    ::AttachThreadInput(::GetWindowThreadProcessId(static_cast<HWND>(Engine::Get<IWindow>()->GetHandle()), nullptr), ::GetCurrentThreadId(), TRUE);
    isInputAttach_ = true;
  }

  BYTE KeyState[kKeyNum];
  //  全てのキーの情報を取得
  if (::GetKeyboardState(KeyState))
  {
    //  全てのキーの情報を更新
    for (vdl::uint i = 0; i < kKeyNum; ++i)
    {
      InputStatus_[i].Update((KeyState[i] & 0x80) != 0);
    }
  }
}
