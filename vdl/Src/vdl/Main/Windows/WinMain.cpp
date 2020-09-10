#include "MsgProc/MsgProc.hpp"

#include <vdl/Fwd.hpp>
#include <vdl/Engine.hpp>

#include <vdl/Resource/Windows/Resource.hpp>
#include <vdl/Constants/Constants.hpp>

#include <crtdbg.h>
#include <locale>

void Main();

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nCmdShow)
{
  //  メモリリーク検出
#if defined _DEBUG
  ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  //  日本語に設定
  ::setlocale(LC_CTYPE, "jpn");

  //  時間で乱数を設定
  ::srand(static_cast<unsigned int>(time(nullptr)));

  //  ウィンドウクラスの登録
  WNDCLASSEX WindowClass;
  {
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_CLASSDC;
    WindowClass.lpfnWndProc = ::WndProc;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = hInstance;
    WindowClass.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    WindowClass.hCursor = nullptr;
    WindowClass.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
    WindowClass.lpszMenuName = nullptr;
    WindowClass.lpszClassName = Constants::kClassName;
    WindowClass.hIconSm = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINI_ICON));
  }
  ::RegisterClassEx(&WindowClass);

  try
  {
    //  エンジンの作成
    Engine Engine;

    //  メイン関数の実行
    Main();
  }
  catch (const std::exception& e)
  {
    ::OutputDebugStringA(e.what());
  }

  ::UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);

  return 0;
}
