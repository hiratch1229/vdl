#include <vdl/Engine.hpp>
#include <vdl/System/ISystem.hpp>
#include <vdl/Input/Keyboard/IKeyboard.hpp>
#include <vdl/Input/Mouse/IMouse.hpp>

#include <vdl/Resource/Windows/Resource.hpp>
#include <vdl/pch/Windows/pch.hpp>

#include <vdl/System.hpp>

#include <crtdbg.h>
#include <locale>
#include <future>

void Main();

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_DESTROY:
    ::PostQuitMessage(0);
    break;
  case WM_PAINT:
    ::ValidateRect(hwnd, NULL);
    break;
  case WM_CLOSE:
    Engine::Get<ISystem>()->EnableDefaultActions(vdl::System::DefaultActionFlag::eExit);
    Engine::Get<ISystem>()->ReportDefaultActions(vdl::System::DefaultActionFlag::eExit);
    Engine::Get<ISystem>()->DisableDefaultActions(vdl::System::DefaultActionFlag::eExit);
    break;
    //  �L�[����������
  case WM_KEYDOWN:
    switch (wParam)
    {
    case VK_ESCAPE:
      Engine::Get<ISystem>()->ReportDefaultActions(vdl::System::DefaultActionFlag::eExit);
      break;
    case VK_F2:
      Engine::Get<ISystem>()->ReportDefaultActions(vdl::System::DefaultActionFlag::eScreenShot);
      break;
    case VK_F11:
      Engine::Get<ISystem>()->ReportDefaultActions(vdl::System::DefaultActionFlag::eChangeWindowMode);
      break;
    }
    break;
    //  �}�E�X�z�C�[��������]
  case WM_MOUSEWHEEL:
    Engine::Get<IMouse>()->Scroll(vdl::int2(0, GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA));
    break;
    //  �}�E�X�z�C�[��������]
  case WM_MOUSEHWHEEL:
    Engine::Get<IMouse>()->Scroll(vdl::int2(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA, 0));
    break;
  case WM_ENTERSIZEMOVE:
    Engine::Get<ISystem>()->Pause();
    break;
  case WM_EXITSIZEMOVE:
    Engine::Get<ISystem>()->Resume();
    break;
  default:
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

  return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
  //  ���������[�N���o
#if defined(DEBUG) | defined(_DEBUG)
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
    WindowClass.lpszClassName = L"vdl";
    WindowClass.hIconSm = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINI_ICON));
  }
  ::RegisterClassEx(&WindowClass);

  try
  {
    //  �G���W���̍쐬
    Engine Engine;

    const std::future<void> Future = std::async(std::launch::async, Main);

    MSG Msg = {};
    while (!Future._Is_ready())
    {
      //  ���b�Z�[�W��S�ď���
      while (::PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
      {
        ::TranslateMessage(&Msg);
        ::DispatchMessage(&Msg);
      }
    }
  }
  catch (const std::exception& e)
  {
    ::OutputDebugStringA(e.what());
  }

  return 0;
}
