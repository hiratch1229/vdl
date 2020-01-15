#include "CKeyboard.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Platform/IPlatform.hpp>
#include <vdl/Window/Windows/CWindow.hpp>

#include <vdl/DetectMemoryLeak.hpp>

#include <vdl/pch/Windows/pch.hpp>

void CKeyboard::Update()
{
  if (!isInputAttach_)
  {
    ::AttachThreadInput(::GetWindowThreadProcessId(Cast<CWindow>(Engine::Get<IWindow>())->GetHandle(), nullptr), ::GetCurrentThreadId(), TRUE);
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
