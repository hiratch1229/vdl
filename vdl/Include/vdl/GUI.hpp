#pragma once
#include "Fwd.hpp"
#include "Type2.hpp"
#include "Type3.hpp"
#include "Type4.hpp"
#include "Color.hpp"
#include "Flag.hpp"

namespace vdl::GUI
{
  // Flags for Begin()
  enum class WindowFlag : uint
  {
    eNone = 0,
    eNoTitleBar = 1 << 0,                  // Disable title-bar
    eNoResize = 1 << 1,                    // Disable user resizing with the lower-right grip
    eNoMove = 1 << 2,                      // Disable user moving the window
    eNoScrollbar = 1 << 3,                 // Disable scrollbars (window can still scroll with mouse or programmatically)
    eNoScrollWithMouse = 1 << 4,           // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
    eNoCollapse = 1 << 5,                  // Disable user collapsing window by double-clicking on it. Also referred to as "window menu button" within a docking node.
    eAlwaysAutoResize = 1 << 6,            // Resize every window to its content every frame
    eNoBackground = 1 << 7,                // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
    eNoSavedSettings = 1 << 8,             // Never load/save settings in .ini file
    eNoMouseInputs = 1 << 9,               // Disable catching mouse, hovering test with pass through.
    eMenuBar = 1 << 10,                    // Has a menu-bar
    eHorizontalScrollbar = 1 << 11,        // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(float2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
    eNoFocusOnAppearing = 1 << 12,         // Disable taking focus when transitioning from hidden to visible state
    eNoBringToFrontOnFocus = 1 << 13,      // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
    eAlwaysVerticalScrollbar = 1 << 14,    // Always show vertical scrollbar (even if ContentSize.y < Size.y)
    eAlwaysHorizontalScrollbar = 1 << 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
    eAlwaysUseWindowPadding = 1 << 16,     // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
    eNoNavInputs = 1 << 18,                // No gamepad/keyboard navigation within the window
    eNoNavFocus = 1 << 19,                 // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
    eUnsavedDocument = 1 << 20,            // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. When used in a tab/docking context, tab is selected on closure and closure is deferred by one frame to allow code to cancel the closure (with a confirmation popup, etc.) without flicker.
    eNoDocking = 1 << 21,                  // Disable docking of this window

    eNoNav = eNoNavInputs | eNoNavFocus,
    eNoDecoration = eNoTitleBar | eNoResize | eNoScrollbar | eNoCollapse,
    eNoInputs = eNoMouseInputs | eNoNavInputs | eNoNavFocus,
  };

  // Flags for InputText()
  enum class InputTextFlag : uint
  {
    eNone = 0,
    eCharsDecimal = 1 << 0,   // Allow 0123456789.+-*/
    eCharsHexadecimal = 1 << 1,   // Allow 0123456789ABCDEFabcdef
    eCharsUppercase = 1 << 2,   // Turn a..z into A..Z
    eCharsNoBlank = 1 << 3,   // Filter out spaces, tabs
    eAutoSelectAll = 1 << 4,   // Select entire text when first taking mouse focus
    eEnterReturnsTrue = 1 << 5,   // Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider looking at the IsItemDeactivatedAfterEdit() function.
    eCallbackCompletion = 1 << 6,   // Callback on pressing TAB (for completion handling)
    eCallbackHistory = 1 << 7,   // Callback on pressing Up/Down arrows (for history handling)
    eCallbackAlways = 1 << 8,   // Callback on each iteration. User code may query cursor position, modify text buffer.
    eCallbackCharFilter = 1 << 9,   // Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
    eAllowTabInput = 1 << 10,  // Pressing TAB input a '\t' character into the text field
    eCtrlEnterForNewLine = 1 << 11,  // In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
    eNoHorizontalScroll = 1 << 12,  // Disable following the cursor horizontally
    eAlwaysInsertMode = 1 << 13,  // Insert mode
    eReadOnly = 1 << 14,  // Read-only mode
    ePassword = 1 << 15,  // Password mode, display all characters as '*'
    eNoUndoRedo = 1 << 16,  // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().
    eCharsScientific = 1 << 17,  // Allow 0123456789.+-*/eE (Scientific notation input)
    eCallbackResize = 1 << 18,  // Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. Notify when the string wants to be resized (for string types which hold a cache of their Size). You will be provided a new BufSize in the callback and NEED to honor it. (see misc/cpp/imgui_stdlib.h for an example of using this)
  };

  // Flags for TreeNodeEx(), CollapsingHeader*()
  enum class TreeNodeFlag : uint16_t
  {
    eNone = 0,
    eSelected = 1 << 0,   // Draw as selected
    eFramed = 1 << 1,   // Full colored frame (e.g. for CollapsingHeader)
    eAllowItemOverlap = 1 << 2,   // Hit testing to allow subsequent widgets to overlap this one
    eNoTreePushOnOpen = 1 << 3,   // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
    eNoAutoOpenOnLog = 1 << 4,   // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
    eDefaultOpen = 1 << 5,   // Default node to be open
    eOpenOnDoubleClick = 1 << 6,   // Need double-click to open node
    eOpenOnArrow = 1 << 7,   // Only open when clicking on the arrow part. If eOpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
    eLeaf = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).
    eBullet = 1 << 9,   // Display a bullet instead of arrow
    eFramePadding = 1 << 10,  // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
    eSpanAvailWidth = 1 << 11,  // Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line. In the future we may refactor the hit system to be front-to-back, allowing natural overlaps and then this can become the default.
    eSpanFullWidth = 1 << 12,  // Extend hit box to the left-most and right-most edges (bypass the indented area).
    eNavLeftJumpsBackHere = 1 << 13,  // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)

    eCollapsingHeader = eFramed | eNoTreePushOnOpen | eNoAutoOpenOnLog
  };

  // Flags for Selectable()
  enum class SelectableFlag : uint8_t
  {
    eNone = 0,
    eDontClosePopups = 1 << 0,  // Clicking this don't close parent popup window
    eSpanAllColumns = 1 << 1,   // Selectable frame can span all columns (text will still fit in current column)
    eAllowDoubleClick = 1 << 2, // Generate press events on double clicks too
    eDisabled = 1 << 3,         // Cannot be selected, display grayed out text
    eAllowItemOverlap = 1 << 4  // (WIP) Hit testing to allow subsequent widgets to overlap this one
  };

  // Flags for BeginCombo()
  enum class ComboFlag : uint8_t
  {
    eNone = 0,
    ePopupAlignLeft = 1 << 0,   // Align the popup toward the left by default
    eHeightSmall = 1 << 1,   // Max ~4 items visible. Tip: If you want your combo popup to be a specific size you can use SetNextWindowSizeConstraints() prior to calling BeginCombo()
    eHeightRegular = 1 << 2,   // Max ~8 items visible (default)
    eHeightLarge = 1 << 3,   // Max ~20 items visible
    eHeightLargest = 1 << 4,   // As many fitting items as possible
    eNoArrowButton = 1 << 5,   // Display on the preview box without the square arrow button
    eNoPreview = 1 << 6,   // Display only a square arrow button
    eHeightMask_ = eHeightSmall | eHeightRegular | eHeightLarge | eHeightLargest
  };

  // Flags for BeginTabBar()
  enum class TabBarFlag : uint8_t
  {
    eNone = 0,
    eReorderable = 1 << 0,                  // Allow manually dragging tabs to re-order them + New tabs are appended at the end of list
    eAutoSelectNewTabs = 1 << 1,            // Automatically select new tabs when they appear
    eTabListPopupButton = 1 << 2,           // Disable buttons to open the tab list popup
    eNoCloseWithMiddleMouseButton = 1 << 3, // Disable behavior of closing tabs (that are submitted with p_open != nullptr) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
    eNoTabListScrollingButtons = 1 << 4,    // Disable scrolling buttons (apply when fitting policy is eFittingPolicyScroll)
    eNoTooltip = 1 << 5,                    // Disable tooltips when hovering a tab
    eFittingPolicyResizeDown = 1 << 6,      // Resize tabs when they don't fit
    eFittingPolicyScroll = 1 << 7,          // Add scroll buttons when tabs don't fit
    eFittingPolicyMask_ = eFittingPolicyResizeDown | eFittingPolicyScroll,
    eFittingPolicyDefault_ = eFittingPolicyResizeDown
  };

  // Flags for BeginTabItem()
  enum class TabItemFlag : uint8_t
  {
    eNone = 0,
    eUnsavedDocument = 1 << 0,              // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. Also: tab is selected on closure and closure is deferred by one frame to allow code to undo it without flicker.
    eSetSelected = 1 << 1,                  // Trigger flag to programmatically make the tab selected when calling BeginTabItem()
    eNoCloseWithMiddleMouseButton = 1 << 2, // Disable behavior of closing tabs (that are submitted with p_open != nullptr) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
    eNoPushId = 1 << 3                      // Don't call PushID(tab->ID)/PopID() on BeginTabItem()/EndTabItem()
  };

  // Flags for IsWindowFocused()
  enum class FocusedFlag : uint8_t
  {
    eNone = 0,
    eChildWindows = 1 << 0, // IsWindowFocused(): Return true if any children of the window is focused
    eRootWindow = 1 << 1,   // IsWindowFocused(): Test from root window (top most parent of the current hierarchy)
    eAnyWindow = 1 << 2,    // IsWindowFocused(): Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use ImGui::GetIO().WantCaptureMouse instead.
    eRootAndChildWindows = eRootWindow | eChildWindows
  };

  // Flags for IsItemHovered(), IsWindowHovered()
  // Note: windows with the NoInputs flag are ignored by IsWindowHovered() calls.
  enum class HoveredFlag : uint8_t
  {
    eNone = 0,        // Return true if directly over the item/window, not obstructed by another window, not obstructed by an active popup or modal blocking inputs under them.
    eChildWindows = 1 << 0,   // IsWindowHovered() only: Return true if any children of the window is hovered
    eRootWindow = 1 << 1,   // IsWindowHovered() only: Test from root window (top most parent of the current hierarchy)
    eAnyWindow = 1 << 2,   // IsWindowHovered() only: Return true if any window is hovered
    eAllowWhenBlockedByPopup = 1 << 3,   // Return true even if a popup window is normally blocking access to this item/window
    //eAllowWhenBlockedByModal     = 1 << 4,   // Return true even if a modal popup window is normally blocking access to this item/window. FIXME-TODO: Unavailable yet.
    eAllowWhenBlockedByActiveItem = 1 << 5,   // Return true even if an active item is blocking access to this item/window. Useful for Drag and Drop patterns.
    eAllowWhenOverlapped = 1 << 6,   // Return true even if the position is obstructed or overlapped by another window
    eAllowWhenDisabled = 1 << 7,   // Return true even if the item is disabled

    eRectOnly = eAllowWhenBlockedByPopup | eAllowWhenBlockedByActiveItem | eAllowWhenOverlapped,
    eRootAndChildWindows = eRootWindow | eChildWindows
  };

  // Flags for ColorEdit3() / ColorEdit4() / ColorPicker3() / ColorPicker4() / ColorButton()
  enum class ColorEditFlag : uint
  {
    eNone = 0,
    eNoAlpha = 1 << 1,   //              // ColorEdit, ColorPicker, ColorButton: ignore Alpha component (will only read 3 components from the input pointer).
    eNoPicker = 1 << 2,   //              // ColorEdit: disable picker when clicking on colored square.
    eNoOptions = 1 << 3,   //              // ColorEdit: disable toggling options menu when right-clicking on inputs/small preview.
    eNoSmallPreview = 1 << 4,   //              // ColorEdit, ColorPicker: disable colored square preview next to the inputs. (e.g. to show only the inputs)
    eNoInputs = 1 << 5,   //              // ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to show only the small preview colored square).
    eNoTooltip = 1 << 6,   //              // ColorEdit, ColorPicker, ColorButton: disable tooltip when hovering the preview.
    eNoLabel = 1 << 7,   //              // ColorEdit, ColorPicker: disable display of inline text label (the label is still forwarded to the tooltip and picker).
    eNoSidePreview = 1 << 8,   //              // ColorPicker: disable bigger color preview on right side of the picker, use small colored square preview instead.
    eNoDragDrop = 1 << 9,   //              // ColorEdit: disable drag and drop target. ColorButton: disable drag and drop source.

    // User Options (right-click on widget to change some of them).
    eAlphaBar = 1 << 16,  //              // ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.
    eAlphaPreview = 1 << 17,  //              // ColorEdit, ColorPicker, ColorButton: display preview as a transparent color over a checkerboard, instead of opaque.
    eAlphaPreviewHalf = 1 << 18,  //              // ColorEdit, ColorPicker, ColorButton: display half opaque / half checkerboard, instead of opaque.
    eHDR = 1 << 19,  //              // (WIP) ColorEdit: Currently only disable 0.0f..1.0f limits in RGBA edition (note: you probably want to use eFloat flag as well).
    eDisplayRGB = 1 << 20,  // [Display]    // ColorEdit: override _display_ type among RGB/HSV/Hex. ColorPicker: select any combination using one or more of RGB/HSV/Hex.
    eDisplayHSV = 1 << 21,  // [Display]    // "
    eDisplayHex = 1 << 22,  // [Display]    // "
    eUint8 = 1 << 23,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255.
    eFloat = 1 << 24,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0.0f..1.0f floats instead of 0..255 integers. No round-trip of value via integers.
    ePickerHueBar = 1 << 25,  // [Picker]     // ColorPicker: bar for Hue, rectangle for Sat/Value.
    ePickerHueWheel = 1 << 26,  // [Picker]     // ColorPicker: wheel for Hue, triangle for Sat/Value.
    eInputRGB = 1 << 27,  // [Input]      // ColorEdit, ColorPicker: input and output data in RGB format.
    eInputHSV = 1 << 28,  // [Input]      // ColorEdit, ColorPicker: input and output data in HSV format.

    // Defaults Options. You can set application defaults using SetColorEditOptions(). The intent is that you probably don't want to
    // override them in most of your calls. Let the user choose via the option menu and/or call SetColorEditOptions() once during startup.
    e_OptionsDefault = eUint8 | eDisplayRGB | eInputRGB | ePickerHueBar,
  };

  // Flags for DragFloat(), DragInt(), SliderFloat(), SliderInt() etc.
  // We use the same sets of flags for DragXXX() and SliderXXX() functions as the features are the same and it makes it easier to swap them.
  enum class SliderFlag : uint
  {
    eNone = 0,
    eClampOnInput = 1 << 4,       // Clamp value to min/max bounds when input manually with CTRL+Click. By default CTRL+Click allows going out of bounds.
    eLogarithmic = 1 << 5,       // Make the widget logarithmic (linear otherwise). Consider using ImGuiSliderFlags_NoRoundToFormat with this if using a format-string with small amount of digits.
    eNoRoundToFormat = 1 << 6,       // Disable rounding underlying value to match precision of the display format string (e.g. %.3f values are rounded to those 3 digits)
    eNoInput = 1 << 7,       // Disable CTRL+Click or Enter key allowing to input text directly into the widget
    eInvalidMask_ = 0x7000000F    // [Internal] We treat using those bits as being potentially a 'float power' argument from the previous API that has got miscast to this enum, and will trigger an assert if needed.
  };

  // Enumateration for ImGui::SetWindow***(), SetNextWindow***(), SetNextItem***() functions
  // Represent a condition.
  enum class Condition
  {
    eAlways,        // Set the variable
    eOnce,          // Set the variable once per runtime session (only the first call with succeed)
    eFirstUseEver,  // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
    eAppearing      // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
  };

  // A cardinal direction
  enum class DirectionType
  {
    eNone = -1,
    eLeft = 0,
    eRight = 1,
    eUp = 2,
    eDown = 3,
  };

  // Windows
  // - Begin() = push window to the stack and start appending to it. End() = pop window from the stack.
  // - You may append multiple times to the same window during the same frame.
  // - Passing 'bool* _pOpen != nullptr' shows a window-closing widget in the upper-right corner of the window,
  //   which clicking will set the boolean to false when clicked.
  // - Begin() return false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting
  //   anything to the window. Always call a matching End() for each Begin() call, regardless of its return value!
  //   [Important: due to legacy reason, this is inconsistent with most other functions such as BeginMenu/EndMenu,
  //    BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function
  //    returned true. Begin and BeginChild are the only odd ones out. Will be fixed in a future update.]
  // - Note that the bottom of window stack always contains a window called "Debug".
  bool Begin(const char* _Name, bool* _pOpen = nullptr, const WindowFlags& _Flags = WindowFlag::eNone);
  void End();

  // Child Windows
  // - Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window. Child windows can embed their own child.
  // - For each independent axis of 'size': ==0.0f: use remaining host window size / >0.0f: fixed size / <0.0f: use remaining window size minus abs(size) / Each axis can use a different mode, e.g. float2(0,400).
  // - BeginChild() returns false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting anything to the window.
  //   Always call a matching EndChild() for each BeginChild() call, regardless of its return value [as with Begin: this is due to legacy reason and inconsistent with most BeginXXX functions apart from the regular Begin() which behaves like BeginChild().]
  bool BeginChild(const char* _StrID, const float2& _Size = float2(0.0f, 0.0f), bool _isBorder = false, const WindowFlags& _Flags = WindowFlag::eNone);
  void EndChild();

  // Windows Utilities
  // - 'current window' = the window we are appending into while inside a Begin()/End() block. 'next window' = next window we will Begin() into.
  bool IsWindowAppearing();
  bool IsWindowCollapsed();
  bool IsWindowDocked();
  bool IsWindowFocused(const FocusedFlags& _Flags = FocusedFlag::eNone);  // is current window focused? or its root/child, depending on flags. see flags for options.
  bool IsWindowHovered(const HoveredFlags& _Flags = HoveredFlag::eNone);  // is current window hovered (and typically: not blocked by a popup/modal)? see flags for options. NB: If you are trying to check whether your mouse should be dispatched to imgui or to your app, you should use the 'io.WantCaptureMouse' boolean for that! Please read the FAQ!
  float2 GetWindowPos();                                                  // get current window position in screen space (useful if you want to do your own drawing via the DrawList API)
  float2 GetWindowSize();                                                 // get current window size
  float GetWindowWidth();                                                 // get current window width (shortcut for GetWindowSize().x)
  float GetWindowHeight();                                                // get current window height (shortcut for GetWindowSize().y)

  // Prefer using SetNextXXX functions (before Begin) rather that SetXXX functions (after Begin).
  void SetNextWindowPos(const float2& _Pos, Condition _Condition = Condition::eAlways, const float2& _Pivot = float2(0.0f, 0.0f));  // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
  void SetNextWindowSize(const float2& _Size, Condition _Condition = Condition::eAlways);                                           // set next window size. set axis to 0.0f to force an auto-fit on this axis. call before Begin()
  void SetNextWindowContentSize(const float2& _Size);                                                                               // set next window content size (~ scrollable client area, which enforce the range of scrollbars). Not including window decorations (title bar, menu bar, etc.) nor WindowPadding. set an axis to 0.0f to leave it automatic. call before Begin()
  void SetNextWindowCollapsed(bool _isCollapsed, Condition _Condition = Condition::eAlways);                                        // set next window collapsed state. call before Begin()
  void SetNextWindowFocus();                                                                                                        // set next window to be focused / top-most. call before Begin()
  void SetNextWindowBgAlpha(float _Alpha);                                                                                          // set next window background color alpha. helper to easily modify ImGuiCol_WindowBg/ChildBg/PopupBg. you may also use ImGuiWindowFlags_NoBackground.
  void SetWindowPos(const float2& _Pos, Condition _Condition = Condition::eAlways);                                                 // (not recommended) set current window position - call within Begin()/End(). prefer using SetNextWindowPos(), as this may incur tearing and side-effects.
  void SetWindowSize(const float2& _Size, Condition _Condition = Condition::eAlways);                                               // (not recommended) set current window size - call within Begin()/End(). set to float2(0,0) to force an auto-fit. prefer using SetNextWindowSize(), as this may incur tearing and minor side-effects.
  void SetWindowCollapsed(bool _isCollapsed, Condition _Condition = Condition::eAlways);                                            // (not recommended) set current window collapsed state. prefer using SetNextWindowCollapsed().
  void SetWindowFocus();                                                                                                            // (not recommended) set current window to be focused / top-most. prefer using SetNextWindowFocus().
  void SetWindowFontScale(float _Scale);                                                                                            // set font scale. Adjust IO.FontGlobalScale if you want to scale all windows. This is an old API! For correct scaling, prefer to reload font + rebuild ImFontAtlas + call style.ScaleAllSizes().
  void SetWindowPos(const char* _Name, const float2& _Pos, Condition _Condition = Condition::eAlways);                              // set named window position.
  void SetWindowSize(const char* _Name, const float2& _Size, Condition _Condition = Condition::eAlways);                            // set named window size. set axis to 0.0f to force an auto-fit on this axis.
  void SetWindowCollapsed(const char* _Name, bool _isCollapsed, Condition _Condition = Condition::eAlways);                         // set named window collapsed state
  void SetWindowFocus(const char* _Name);                                                                                           // set named window to be focused / top-most. use nullptr to remove focus.

  // Content region
  // - Those functions are bound to be redesigned soon (they are confusing, incomplete and return values in local window coordinates which increases confusion)
  float2 GetContentRegionMax();         // current content boundaries (typically window boundaries including scrolling, or current column boundaries), in windows coordinates
  float2 GetContentRegionAvail();       // == GetContentRegionMax() - GetCursorPos()
  float2 GetWindowContentRegionMin();   // content boundaries min (roughly (0,0)-Scroll), in window coordinates
  float2 GetWindowContentRegionMax();   // content boundaries max (roughly (0,0)+Size-Scroll) where Size can be override with SetNextWindowContentSize(), in window coordinates
  float GetWindowContentRegionWidth();  //

  // Windows Scrolling
  float GetScrollX();                                                 // get scrolling amount [0..GetScrollMaxX()]
  float GetScrollY();                                                 // get scrolling amount [0..GetScrollMaxY()]
  float GetScrollMaxX();                                              // get maximum scrolling amount ~~ ContentSize.X - WindowSize.X
  float GetScrollMaxY();                                              // get maximum scrolling amount ~~ ContentSize.Y - WindowSize.Y
  void SetScrollX(float _ScrollX);                                    // set scrolling amount [0..GetScrollMaxX()]
  void SetScrollY(float _ScrollY);                                    // set scrolling amount [0..GetScrollMaxY()]
  void SetScrollHereX(float _CenterRatioX = 0.5f);                    // adjust scrolling amount to make current cursor position visible. center_x_ratio=0.0: left, 0.5: center, 1.0: right. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead.
  void SetScrollHereY(float _CenterRatioY = 0.5f);                    // adjust scrolling amount to make current cursor position visible. center_y_ratio=0.0: top, 0.5: center, 1.0: bottom. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead.
  void SetScrollFromPosX(float _LocalX, float _CenterRatioX = 0.5f);  // adjust scrolling amount to make given position visible. Generally GetCursorStartPos() + offset to compute a valid position.
  void SetScrollFromPosY(float _LocalY, float _CenterRatioY = 0.5f);  // adjust scrolling amount to make given position visible. Generally GetCursorStartPos() + offset to compute a valid position.

  // Parameters stacks (current window)
  void PushItemWidth(float _ItemWidth);                   // set width of items for common large "item+label" widgets. >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -1.0f always align width to the right side). 0.0f = default to ~2/3 of windows width,
  void PopItemWidth();
  void SetNextItemWidth(float _ItemWidth);                // set width of the _next_ common large "item+label" widget. >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -1.0f always align width to the right side)
  float CalcItemWidth();                                  // width of item given pushed settings and current cursor position. NOT necessarily the width of last item unlike most 'Item' functions.
  void PushTextWrapPos(float _WrapLocalPosX = 0.0f);      // word-wrapping for Text*() commands. < 0.0f: no wrapping; 0.0f: wrap to end of window (or column); > 0.0f: wrap at 'wrap_pos_x' position in window local space
  void PopTextWrapPos();
  void PushAllowKeyboardFocus(bool _AllowKeyboardFocus);  // allow focusing using TAB/Shift-TAB, enabled by default but you can disable it for certain widgets
  void PopAllowKeyboardFocus();
  void PushButtonRepeat(bool _Repeat);                    // in 'repeat' mode, Button*() functions return repeated true in a typematic manner (using io.KeyRepeatDelay/io.KeyRepeatRate setting). Note that you can call IsItemActive() after any Button() to tell if the button is held in the current frame.
  void PopButtonRepeat();

  // Cursor / Layout
  // - By "cursor" we mean the current output position.
  // - The typical widget behavior is to output themselves at the current cursor position, then move the cursor one line down.
  // - You can call SameLine() between widgets to undo the last carriage return and output at the right of the preceeding widget.
  void Separator();                                                       // separator, generally horizontal. inside a menu bar or in horizontal layout mode, this becomes a vertical separator.
  void SameLine(float _OffsetFromStartX = 0.0f, float _Spacing = -1.0f);  // call between widgets or groups to layout them horizontally. X position given in window coordinates.
  void NewLine();                                                         // undo a SameLine() or force a new line when in an horizontal-layout context.
  void Spacing();                                                         // add vertical spacing.
  void Dummy(const float2& _Size);                                        // add a dummy item of given size. unlike InvisibleButton(), Dummy() won't take the mouse click or be navigable into.
  void Indent(float _IndentW = 0.0f);                                     // move content position toward the right, by style.IndentSpacing or indent_w if != 0
  void Unindent(float _IndentW = 0.0f);                                   // move content position back to the left, by style.IndentSpacing or indent_w if != 0
  void BeginGroup();                                                      // lock horizontal starting position
  void EndGroup();                                                        // unlock horizontal starting position + capture the whole group bounding box into one "item" (so you can use IsItemHovered() or layout primitives such as SameLine() on whole group, etc.)
  void AlignTextToFramePadding();                                         // vertically align upcoming text baseline to FramePadding.y so that it will align properly to regularly framed items (call if you have text on a line before a framed item)
  float GetTextLineHeight();                                              // ~ FontSize
  float GetTextLineHeightWithSpacing();                                   // ~ FontSize + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of text)
  float GetFrameHeight();                                                 // ~ FontSize + style.FramePadding.y * 2
  float GetFrameHeightWithSpacing();                                      // ~ FontSize + style.FramePadding.y * 2 + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of framed widgets)

  // Widgets: Text
  void TextUnformatted(const char* _Text, const char* _TextEnd = nullptr);  // raw text without formatting. Roughly equivalent to Text("%s", text) but: A) doesn't require nullptr terminated string if 'text_end' is specified, B) it's faster, no memory copy is done, no buffer size limits, recommended for long chunks of text.
  void Text(const char* _Format, ...);                                      // formatted text
  void TextColored(const Color4F& _Color, const char* _Format, ...);         // shortcut for PushStyleColor(ImGuiCol_Text, col); Text(fmt, ...); PopStyleColor();
  void TextDisabled(const char* _Format, ...);                              // shortcut for PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]); Text(fmt, ...); PopStyleColor();
  void TextWrapped(const char* _Format, ...);                               // shortcut for PushTextWrapPos(0.0f); Text(fmt, ...); PopTextWrapPos();. Note that this won't work on an auto-resizing window if there's no other widgets to extend the window width, yoy may need to set a size using SetNextWindowSize().
  void LabelText(const char* _Label, const char* _Format, ...);             // display text+label aligned the same way as value+label widgets
  void BulletText(const char* _Format, ...);                                // shortcut for Bullet()+Text()

  // Widgets: Main
  // - Most widgets return true when the value has been changed or when pressed/selected
  // - You may also use one of the many IsItemXXX functions (e.g. IsItemActive, IsItemHovered, etc.) to query widget state.
  bool Button(const char* _Label, const float2& _Size = float2(0.0f, 0.0f));    // button
  bool SmallButton(const char* _Label);                                 // button with FramePadding=(0,0) to easily embed within text
  bool InvisibleButton(const char* _StrID, const float2& _Size);        // button behavior without the visuals, frequently useful to build custom behaviors using the public api (along with IsItemActive, IsItemHovered, etc.)
  bool ArrowButton(const char* _StrID, DirectionType _Type);                  // square button with an arrow shape
  void Image(const Texture& _Texture, const float2& _Size, const float2& _UV0 = float2(0.0f, 0.0f), const float2& _UV1 = float2(1.0f, 1.0f), const Color4F& _TintColor = Palette::White, const Color4F& _BorderColor = Palette::Black);
  bool ImageButton(const Texture& _Texture, const float2& _Size, const float2& _UV0 = float2(0.0f, 0.0f), const float2& _UV1 = float2(1.0f, 1.0f), int _FramePadding = -1, const Color4F& _BackgroundColor = Palette::Black, const Color4F& _TintColor = Palette::White);    // <0 frame_padding uses default frame padding settings. 0 for no padding
  bool Checkbox(const char* _Label, bool* _Flag);
  bool CheckboxFlags(const char* _Label, uint* _Flags, uint _FlagsValue);
  bool RadioButton(const char* _Label, bool _Active);                    // use with e.g. if (RadioButton("one", my_value==1)) { my_value = 1; }
  void ProgressBar(float _Fraction, const float2& _SizeArg = float2(-1.0f, 0.0f), const char* _Overlay = nullptr);
  void Bullet();                                                       // draw a small circle and keep the cursor on the same line. advance cursor x position by GetTreeNodeToLabelSpacing(), same distance that TreeNode() uses

  // Widgets: Combo Box
  // - The BeginCombo()/EndCombo() api allows you to manage your contents and selection state however you want it, by creating e.g. Selectable() items.
  // - The old Combo() api are helpers over BeginCombo()/EndCombo() which are kept available for convenience purpose.
  bool BeginCombo(const char* _Label, const char* _PreviewValue, const ComboFlags& _Flags = ComboFlag::eNone);
  void EndCombo();                                                                                                                // only call EndCombo() if BeginCombo() returns true!
  bool Combo(const char* _Label, int* _pCurrentItem, const char* const _Items[], int _ItemNum, int _PopupMaxHeightInItems = -1);
  bool Combo(const char* _Label, int* _pCurrentItem, const char* _ItemsSeparatedByZeros, int _PopupMaxHeightInItems = -1);        // Separate items with \0 within a string, end item-list with \0\0. e.g. "One\0Two\0Three\0"
  bool Combo(const char* _Label, int* _pCurrentItem, bool (*_ItemsGetter)(void* _pData, int _Index, const char** _OutText), void* _pData, int _ItemNum, int _PopupMaxHeightInItems = -1);

  // Widgets: Drags
  // - CTRL+Click on any drag box to turn them into an input box. Manually input values aren't clamped and can go off-bounds.
  // - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
  // - Speed are per-pixel of mouse movement (v_speed=0.2f: mouse needs to move by 5 pixels to increase value by 1). For gamepad/keyboard navigation, minimum speed is Max(v_speed, minimum_step_at_given_precision).
  // - Use v_min < v_max to clamp edits to given limits. Note that CTRL+Click manual input can override those limits.
  // - Use v_max = FLT_MAX / INT_MAX etc to avoid clamping to a maximum, same with v_min = -FLT_MAX / INT_MIN to avoid clamping to a minimum.
  // - Use v_min > v_max to lock edits.
  template<class Type> bool Drag(const char* _Label, Type* _pData, float _Speed, Type _Min, Type _Max, const char* _Format, float _Power);
  template<class Type> bool Drag(const char* _Label, Type2<Type>* _pData, float _Speed, Type _Min, Type _Max, const char* _Format, float _Power);
  template<class Type> bool Drag(const char* _Label, Type3<Type>* _pData, float _Speed, Type _Min, Type _Max, const char* _Format, float _Power);
  template<class Type> bool Drag(const char* _Label, Type4<Type>* _pData, float _Speed, Type _Min, Type _Max, const char* _Format, float _Power);

  // Widgets: Sliders
  // - CTRL+Click on any slider to turn them into an input box. Manually input values aren't clamped and can go off-bounds.
  // - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
  template<class Type> bool Slider(const char* _Label, Type* _pData, Type _Min, Type _Max, const char* _Format, float _Power);
  template<class Type> bool Slider(const char* _Label, Type2<Type>* _pData, Type _Min, Type _Max, const char* _Format, float _Power);
  template<class Type> bool Slider(const char* _Label, Type3<Type>* _pData, Type _Min, Type _Max, const char* _Format, float _Power);
  template<class Type> bool Slider(const char* _Label, Type4<Type>* _pData, Type _Min, Type _Max, const char* _Format, float _Power);

  // Widgets: Input with Keyboard
  // - If you want to use InputText() with a dynamic string type such as std::string or your own, see misc/cpp/imgui_stdlib.h
  // - Most of the ImGuiInputTextFlags flags are only useful for InputText() and not for InputFloatX, InputIntX, InputDouble etc.
  bool InputText(const char* _Label, char* _Buffer, size_t _BufferSize, const InputTextFlags& _Flags = InputTextFlag::eNone);
  bool InputTextMultiline(const char* _Label, char* _Buffer, size_t _BufferSize, const float2& _Size = float2(0.0f, 0.0f), const InputTextFlags& _Flags = InputTextFlag::eNone);
  bool InputTextWithHint(const char* _Label, const char* _Hint, char* _Buffer, size_t _BufferSize, const InputTextFlags& _Flags = InputTextFlag::eNone);
  template<class Type> bool Input(const char* _Label, Type* _pData, Type _Step, Type _StepFast, const char* _Format, const InputTextFlags& _Flags);
  template<class Type> bool Input(const char* _Label, Type2<Type>* _pData, const char* _Format, const InputTextFlags& _Flags);
  template<class Type> bool Input(const char* _Label, Type3<Type>* _pData, const char* _Format, const InputTextFlags& _Flags);
  template<class Type> bool Input(const char* _Label, Type4<Type>* _pData, const char* _Format, const InputTextFlags& _Flags);

  // Widgets: Color Editor/Picker (tip: the ColorEdit* functions have a little colored preview square that can be left-clicked to open a picker, and right-clicked to open an option menu.)
  // - Note that in C++ a 'float v[X]' function argument is the _same_ as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible.
  // - You can pass the address of a first float element out of a contiguous structure, e.g. &myvector.x
  bool ColorEdit(const char* _Label, Color3F* _pColor, const ColorEditFlags& _Flags = ColorEditFlag::eNone);
  bool ColorEdit(const char* _Label, Color4F* _pColor, const ColorEditFlags& _Flags = ColorEditFlag::eNone);
  bool ColorPicker(const char* _Label, Color3F* _pColor, const ColorEditFlags& _Flags = ColorEditFlag::eNone);
  bool ColorPicker(const char* _Label, Color4F* _pColor, const ColorEditFlags& _Flags = ColorEditFlag::eNone, const Color4F* _pRefColor = nullptr);
  bool ColorButton(const char* _DescID, const Color4F& _Color, const ColorEditFlags& _Flags = ColorEditFlag::eNone, const float2& _Size = float2(0.0f, 0.0f)); // display a colored square/button, hover for details, return true when pressed.
  void SetColorEditOptions(const ColorEditFlags& _Flags);                                                                                                     // initialize current options (generally on application startup) if you want to select a default format, picker type, etc. User will be able to change many settings, unless you pass the _NoOptions flag to your calls.

  // Widgets: Trees
  // - TreeNode functions return true when the node is open, in which case you need to also call TreePop() when you are finished displaying the tree node contents.
  bool TreeNode(const char* _Label);
  bool TreeNode(const char* _StrID, const char* _Format, ...);                                                // helper variation to easily decorelate the id from the displayed string. Read the FAQ about why and how to use ID. to align arbitrary text at the same level as a TreeNode() you can use Bullet().
  bool TreeNodeEx(const char* _Label, const TreeNodeFlags& _Flags = TreeNodeFlag::eNone);
  bool TreeNodeEx(const char* _StrID, const TreeNodeFlags& _Flags, const char* _Format, ...);
  void TreePush(const char* _StrID);                                                                          // ~ Indent()+PushId(). Already called by TreeNode() when returning true, but you can call TreePush/TreePop yourself if desired.
  void TreePop();                                                                                             // ~ Unindent()+PopId()
  float GetTreeNodeToLabelSpacing();                                                                          // horizontal distance preceding label when using TreeNode*() or Bullet() == (g.FontSize + style.FramePadding.x*2) for a regular unframed TreeNode
  bool CollapsingHeader(const char* _Label, const TreeNodeFlags& _Flags = TreeNodeFlag::eNone);               // if returning 'true' the header is open. doesn't indent nor push on ID stack. user doesn't have to call TreePop().
  bool CollapsingHeader(const char* _Label, bool* _pOpen, const TreeNodeFlags& _Flags = TreeNodeFlag::eNone); // when 'p_open' isn't nullptr, display an additional small close button on upper right of the header
  void SetNextItemOpen(bool _isOpen, Condition _Condition = Condition::eAlways);                              // set next TreeNode/CollapsingHeader open state.

  // Widgets: Selectables
  // - A selectable highlights when hovered, and can display another color when selected.
  // - Neighbors selectable extend their highlight bounds in order to leave no gap between them. This is so a series of selected Selectable appear contiguous.
  bool Selectable(const char* _Label, bool _Selected = false, const SelectableFlags& _Flags = SelectableFlag::eNone, const float2& _Size = float2(0.0f, 0.0f)); // "bool selected" carry the selection state (read-only). Selectable() is clicked is returns true so you can modify your selection state. size.x==0.0: use remaining width, size.x>0.0: specify width. size.y==0.0: use label height, size.y>0.0: specify height
  bool Selectable(const char* _Label, bool* _pSelected, const SelectableFlags& _Flags = SelectableFlag::eNone, const float2& _Size = float2(0.0f, 0.0f));       // "bool* p_selected" point to the selection state (read-write), as a convenient helper.

  // Widgets: List Boxes
  // - FIXME: To be consistent with all the newer API, ListBoxHeader/ListBoxFooter should in reality be called BeginListBox/EndListBox. Will rename them.
  bool ListBox(const char* _Label, int* _pCurrentItem, const char* const _Items[], int _ItemNum, int _HeightInItems = -1);
  bool ListBox(const char* _Label, int* _pCurrentItem, bool (*_ItemsGetter)(void* _pData, int _Index, const char** _OutText), void* _pData, int _ItemNum, int _HeightInItems = -1);
  bool ListBoxHeader(const char* _Label, const float2& _Size = float2(0.0f, 0.0f)); // use if you want to reimplement ListBox() will custom data or interactions. if the function return true, you can output elements then call ListBoxFooter() afterwards.
  bool ListBoxHeader(const char* _Label, int _ItemNum, int _HeightInItems = -1);    // "
  void ListBoxFooter();                                                             // terminate the scrolling region. only call ListBoxFooter() if ListBoxHeader() returned true!

  // Widgets: Data Plotting
  void PlotLines(const char* _Label, const float* _Values, int _ValueNum, int _ValuesOffset = 0, const char* _OverlayText = nullptr, float _ScaleMin = FLT_MAX, float _ScaleMax = FLT_MAX, const float2& _GraphSize = float2(0.0f, 0.0f));
  void PlotLines(const char* _Label, float(*_ValuesGetter)(void* _pData, int _Index), void* _pData, int _ValueNum, int _ValuesOffset = 0, const char* _OverlayText = nullptr, float _ScaleMin = FLT_MAX, float _ScaleMax = FLT_MAX, const float2& _GraphSize = float2(0.0f, 0.0f));
  void PlotHistogram(const char* _Label, const float* _Values, int _ValueNum, int _ValuesOffset = 0, const char* _OverlayText = nullptr, float _ScaleMin = FLT_MAX, float _ScaleMax = FLT_MAX, const float2& _GraphSize = float2(0.0f, 0.0f));
  void PlotHistogram(const char* _Label, float(*_ValuesGetter)(void* _pData, int _Index), void* _pData, int _ValueNum, int _ValuesOffset = 0, const char* _OverlayText = nullptr, float _ScaleMin = FLT_MAX, float _ScaleMax = FLT_MAX, const float2& _GraphSize = float2(0.0f, 0.0f));

  // Widgets: Menus
  // - Use BeginMenuBar() on a window ImGuiWindowFlags_MenuBar to append to its menu bar.
  // - Use BeginMainMenuBar() to create a menu bar at the top of the screen.
  bool BeginMenuBar();                                                                                              // append to menu-bar of current window (requires ImGuiWindowFlags_MenuBar flag set on parent window).
  void EndMenuBar();                                                                                                // only call EndMenuBar() if BeginMenuBar() returns true!
  bool BeginMainMenuBar();                                                                                          // create and append to a full screen menu-bar.
  void EndMainMenuBar();                                                                                            // only call EndMainMenuBar() if BeginMainMenuBar() returns true!
  bool BeginMenu(const char* _Label, bool _Enabled = true);                                                         // create a sub-menu entry. only call EndMenu() if this returns true!
  void EndMenu();                                                                                                   // only call EndMenu() if BeginMenu() returns true!
  bool MenuItem(const char* _Label, const char* _Shortcut = nullptr, bool _Selected = false, bool _Enabled = true); // return true when activated. shortcuts are displayed for convenience but not processed by ImGui at the moment
  bool MenuItem(const char* _Label, const char* _Shortcut, bool* _pSelected, bool _Enabled = true);                 // return true when activated + toggle (*p_selected) if p_selected != nullptr

  // Tooltips
  // - Tooltip are windows following the mouse which do not take focus away.
  void BeginTooltip();                        // begin/append a tooltip window. to create full-featured tooltip (with any kind of items).
  void EndTooltip();
  void SetTooltip(const char* _Format, ...);  // set a text-only tooltip, typically use with ImGui::IsItemHovered(). override any previous call to SetTooltip().

  // Popups, Modals
  // The properties of popups windows are:
  // - They block normal mouse hovering detection outside them. (*)
  // - Unless modal, they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
  // - Their visibility state (~bool) is held internally by imgui instead of being held by the programmer as we are used to with regular Begin() calls.
  //   User can manipulate the visibility state by calling OpenPopup().
  // - We default to use the right mouse (ImGuiMouseButton_Right=1) for the Popup Context functions.
  // (*) You can use IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) to bypass it and detect hovering even when normally blocked by a popup.
  // Those three properties are connected. The library needs to hold their visibility state because it can close popups at any time.
  void OpenPopup(const char* _StrID);                                                                             // call to mark popup as open (don't call every frame!). popups are closed when user click outside, or if CloseCurrentPopup() is called within a BeginPopup()/EndPopup() block. By default, Selectable()/MenuItem() are calling CloseCurrentPopup(). Popup identifiers are relative to the current ID-stack (so OpenPopup and BeginPopup needs to be at the same level).
  bool BeginPopup(const char* _StrID, const WindowFlags& _Flags = WindowFlag::eNone);                             // return true if the popup is open, and you can start outputting to it. only call EndPopup() if BeginPopup() returns true!
  bool BeginPopupContextItem(const char* _StrID = nullptr);                                                       // helper to open and begin popup when clicked on last item. if you can pass a nullptr str_id only if the previous item had an id. If you want to use that on a non-interactive item such as Text() you need to pass in an explicit ID here. read comments in .cpp!
  bool BeginPopupContextWindow(const char* _StrID = nullptr, bool _AlsoOverItems = true);                         // helper to open and begin popup when clicked on current window.
  bool BeginPopupContextVoid(const char* _StrID = nullptr);                                                       // helper to open and begin popup when clicked in void (where there are no imgui windows).
  bool BeginPopupModal(const char* _Name, bool* _pOpen = nullptr, const WindowFlags& _Flags = WindowFlag::eNone); // modal dialog (regular window with title bar, block interactions behind the modal window, can't close the modal window by clicking outside)
  void EndPopup();                                                                                                // only call EndPopup() if BeginPopupXXX() returns true!

  // Columns
  // - You can also use SameLine(pos_x) to mimic simplified columns.
  // - The columns API is work-in-progress and rather lacking (columns are arguably the worst part of dear imgui at the moment!)
  // - There is a maximum of 64 columns.
  // - By end of the 2019 we will expose a new 'Table' api which will replace columns.
  void Columns(int _Count = 1, const char* _StrID = nullptr, bool _Border = true);
  void NextColumn();                                                                // next column, defaults to current row or next row if the current row is finished
  int GetColumnIndex();                                                             // get current column index
  float GetColumnWidth(int _ColumnIndex = -1);                                      // get column width (in pixels). pass -1 to use current column
  void SetColumnWidth(int _ColumnIndex, float _Width);                              // set column width (in pixels). pass -1 to use current column
  float GetColumnOffset(int _ColumnIndex = -1);                                     // get position of column line (in pixels, from the left side of the contents region). pass -1 to use current column, otherwise 0..GetColumnsCount() inclusive. column 0 is typically 0.0f
  void SetColumnOffset(int _ColumnIndex, float _OffsetX);                           // set position of column line (in pixels, from the left side of the contents region). pass -1 to use current column
  int GetColumnsCount();

  // Tab Bars, Tabs
  // Note: Tabs are automatically created by the docking system. Use this to create tab bars/tabs yourself without docking being involved.
  bool BeginTabBar(const char* _StrID, const TabBarFlags& _Flags = TabBarFlag::eNone);                            // create and append into a TabBar
  void EndTabBar();                                                                                               // only call EndTabBar() if BeginTabBar() returns true!
  bool BeginTabItem(const char* _Label, bool* _pOpen = nullptr, const TabItemFlags& _Flags = TabItemFlag::eNone); // create a Tab. Returns true if the Tab is selected.
  void EndTabItem();                                                                                              // only call EndTabItem() if BeginTabItem() returns true!
  void SetTabItemClosed(const char* _TabOrDockedWindowLabel);                                                     // notify TabBar or Docking system of a closed tab/window ahead (useful to reduce visual flicker on reorderable tab bars). For tab-bar: call after BeginTabBar() and before Tab submissions. Otherwise call with a window name.

  // Clipping
  void PushClipRect(const float2& _MinClipRect, const float2& _MaxClipRect, bool _IntersectWithCurrentClipRect);
  void PopClipRect();

  // Focus, Activation
  // - Prefer using "SetItemDefaultFocus()" over "if (IsWindowAppearing()) SetScrollHereY()" when applicable to signify "this is the default item"
  void SetItemDefaultFocus();                 // make last item the default focused item of a window.
  void SetKeyboardFocusHere(int _Offset = 0); // focus keyboard on the next widget. Use positive 'offset' to access sub components of a multiple component widget. Use -1 to access previous widget.

  // Item/Widgets Utilities
  // - Most of the functions are referring to the last/previous item we submitted.
  // - See Demo Window under "Widgets->Querying Status" for an interactive visualization of most of those functions.
  bool IsItemHovered(const HoveredFlags& _Flags = HoveredFlag::eNone);  // is the last item hovered? (and usable, aka not blocked by a popup, etc.). See ImGuiHoveredFlags for more options.
  bool IsItemActive();                                                  // is the last item active? (e.g. button being held, text field being edited. This will continuously return true while holding mouse button on an item. Items that don't interact will always return false)
  bool IsItemFocused();                                                 // is the last item focused for keyboard/gamepad navigation?
  bool IsItemClicked();                                                 // is the last item clicked? (e.g. button/node just clicked on) == IsMouseClicked(mouse_button) && IsItemHovered()
  bool IsItemVisible();                                                 // is the last item visible? (items may be out of sight because of clipping/scrolling)
  bool IsItemEdited();                                                  // did the last item modify its underlying value this frame? or was pressed? This is generally the same as the "bool" return value of many widgets.
  bool IsItemActivated();                                               // was the last item just made active (item was previously inactive).
  bool IsItemDeactivated();                                             // was the last item just made inactive (item was previously active). Useful for Undo/Redo patterns with widgets that requires continuous editing.
  bool IsItemDeactivatedAfterEdit();                                    // was the last item just made inactive and made a value change when it was active? (e.g. Slider/Drag moved). Useful for Undo/Redo patterns with widgets that requires continuous editing. Note that you may get false positives (some widgets such as Combo()/ListBox()/Selectable() will return true even when clicking an already selected item).
  bool IsItemToggledOpen();                                             // was the last item open state toggled? set by TreeNode().
  bool IsAnyItemHovered();                                              // is any item hovered?
  bool IsAnyItemActive();                                               // is any item active?
  bool IsAnyItemFocused();                                              // is any item focused?
  float2 GetItemRectMin();                                              // get upper-left bounding rectangle of the last item (screen space)
  float2 GetItemRectMax();                                              // get lower-right bounding rectangle of the last item (screen space)
  float2 GetItemRectSize();                                             // get size of last item
  void SetItemAllowOverlap();                                           // allow last item to be overlapped by a subsequent item. sometimes useful with invisible buttons, selectables, etc. to catch unused area.

  // Miscellaneous Utilities
  bool IsRectVisible(const float2& _Size);                                  // test if rectangle (of given size, starting from cursor position) is visible / not clipped.
  bool IsRectVisible(const float2& _MinRect, const float2& _MaxRect);      // test if rectangle (in screen space) is visible / not clipped. to perform coarse clipping on user's side.
  float2 CalcTextSize(const char* _Text, const char* _TextEnd = nullptr, bool _HideTextAfterDoubleHash = false, float _WrapWidth = -1.0f);
  void CalcListClipping(int _ItemNum, float _ItemHeight, int* _OutItemDisplayStart, int* _OutItemDisplayEnd);    // calculate coarse clipping for large list of evenly sized items. Prefer using the ImGuiListClipper higher-level helper if you can.

  // Clipboard Utilities (also see the LogToClipboard() function to capture or output text data to the clipboard)
  const char* GetClipboardText();
  void SetClipboardText(const char* _Text);
}

#include "GUI.inl"
