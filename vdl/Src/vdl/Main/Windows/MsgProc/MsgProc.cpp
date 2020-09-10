#include "MsgProc.hpp"

#include <vdl/Engine.hpp>
#include <vdl/System/ISystem.hpp>
#include <vdl/Input/Keyboard/IKeyboard.hpp>
#include <vdl/Input/Mouse/IMouse.hpp>
#include <vdl/Platform/IPlatform.hpp>
#include <vdl/GUI/CGUI.hpp>

#include <vdl/System.hpp>

#include <ThirdParty/ImGui/imgui.h>

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
    //  キーを押した時
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
    //  マウスホイール垂直回転
  case WM_MOUSEWHEEL:
    Engine::Get<IMouse>()->Scroll(vdl::int2(0, GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA));
    break;
    //  マウスホイール水平回転
  case WM_MOUSEHWHEEL:
    Engine::Get<IMouse>()->Scroll(vdl::int2(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA, 0));
    break;
      //  ウィンドウを非選択状態になった時
    case WM_KILLFOCUS:
      //  ウィンドウの移動開始時
    case WM_ENTERSIZEMOVE:
      Engine::Get<ISystem>()->Pause();
      break;
      //  ウィンドウを選択状態になった時
    case WM_SETFOCUS:
      //  ウィンドウの移動終了時
    case WM_EXITSIZEMOVE:
      Engine::Get<ISystem>()->Resume();
      break;
  default:
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

  return 0;
}

LRESULT CALLBACK SubWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (::WndProc(hwnd, uMsg, wParam, lParam))
  {
    return true;
  }

  if (ImGuiViewport* pViewport = ImGui::FindViewportByPlatformHandle(hwnd))
  {
    switch (uMsg)
    {
    case WM_CLOSE:
      pViewport->PlatformRequestClose = true;
      return 0;
    case WM_MOVE:
      pViewport->PlatformRequestMove = true;
      break;
    case WM_SIZE:
      pViewport->PlatformRequestResize = true;
      break;
    case WM_MOUSEACTIVATE:
      if (pViewport->Flags & ImGuiViewportFlags_NoFocusOnClick)
      {
        return MA_NOACTIVATE;
      }
      break;
    case WM_NCHITTEST:
      // Let mouse pass-through the window. This will allow the back-end to set io.MouseHoveredViewport properly (which is OPTIONAL).
      // The ImGuiViewportFlags_NoInputs flag is set while dragging a viewport, as want to detect the window behind the one we are dragging.
      // If you cannot easily access those viewport flags from your windowing/event code: you may manually synchronize its state e.g. in
      // your main loop after calling UpdatePlatformWindows(). Iterate all viewports/platform windows and pass the flag to your windowing system.
      if (pViewport->Flags & ImGuiViewportFlags_NoInputs)
      {
        return HTTRANSPARENT;
      }
      break;
    }
  }

  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
