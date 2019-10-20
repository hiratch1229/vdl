#pragma once
//#include "Fwd.hpp"
//
//namespace vdl::GUI
//{
//  namespace Window
//  {
//    enum Flags
//    {
//      eNone = 0,
//      eNoTitleBar = 1 << 0,   // Disable title-bar
//      eNoResize = 1 << 1,   // Disable user resizing with the lower-right grip
//      eNoMove = 1 << 2,   // Disable user moving the window
//      eNoScrollbar = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
//      eNoScrollWithMouse = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
//      eNoCollapse = 1 << 5,   // Disable user collapsing window by double-clicking on it
//      eAlwaysAutoResize = 1 << 6,   // Resize every window to its content every frame
//      eNoBackground = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
//      eNoSavedSettings = 1 << 8,   // Never load/save settings in .ini file
//      eNoMouseInputs = 1 << 9,   // Disable catching mouse, hovering test with pass through.
//      eMenuBar = 1 << 10,  // Has a menu-bar
//      eHorizontalScrollbar = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
//      eNoFocusOnAppearing = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
//      eNoBringToFrontOnFocus = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
//      eAlwaysVerticalScrollbar = 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
//      eAlwaysHorizontalScrollbar = 1 << 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
//      eAlwaysUseWindowPadding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
//      eNoNavInputs = 1 << 18,  // No gamepad/keyboard navigation within the window
//      eNoNavFocus = 1 << 19,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
//      eUnsavedDocument = 1 << 20,  // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. When used in a tab/docking context, tab is selected on closure and closure is deferred by one frame to allow code to cancel the closure (with a confirmation popup, etc.) without flicker.
//      eNoNav = eNoNavInputs | eNoNavFocus,
//      eNoDecoration = eNoTitleBar | eNoResize | eNoScrollbar | eNoCollapse,
//      eNoInputs = eNoMouseInputs | eNoNavInputs | eNoNavFocus,
//    };
//  }
//
//  bool Begin(const char* _Name, bool* _pOpen = nullptr, Window::Flags _Flags = Window::Flags::eNone);
//
//  void End();
//
//  bool BeginChild();
//
//  bool BeginChild();
//
//  void EndChild();
//}

#include "../ThirdParty/ImGui/imgui.h"
