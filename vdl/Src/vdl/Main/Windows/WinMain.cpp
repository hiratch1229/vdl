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
  //  ���������[�N���o
#if defined _DEBUG
  ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  //  ���{��ɐݒ�
  ::setlocale(LC_CTYPE, "jpn");

  //  ���Ԃŗ�����ݒ�
  ::srand(static_cast<unsigned int>(time(nullptr)));

  //  �E�B���h�E�N���X�̓o�^
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
    //  �G���W���̍쐬
    Engine Engine;

    //  ���C���֐��̎��s
    Main();
  }
  catch (const std::exception& e)
  {
    ::OutputDebugStringA(e.what());
  }

  ::UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);

  return 0;
}
