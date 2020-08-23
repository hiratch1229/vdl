#include <vdl/GUI.hpp>

#include <ThirdParty/ImGui/imgui.h>

namespace
{
  inline ImGuiCond_ Cast(vdl::GUI::Condition _Condition)
  {
    switch (_Condition)
    {
    case vdl::GUI::Condition::eAlways:
      return ImGuiCond_Always;
    case vdl::GUI::Condition::eOnce:
      return ImGuiCond_Once;
    case vdl::GUI::Condition::eFirstUseEver:
      return ImGuiCond_FirstUseEver;
    case vdl::GUI::Condition::eAppearing:
      return ImGuiCond_Appearing;
    default: assert(false);
    }

    return ImGuiCond_Always;
  }

  inline ImGuiDir_ Cast(vdl::GUI::DirectionType _Type)
  {
    switch (_Type)
    {
    case vdl::GUI::DirectionType::eNone:
      return ImGuiDir_None;
    case vdl::GUI::DirectionType::eLeft:
      return ImGuiDir_Left;
    case vdl::GUI::DirectionType::eRight:
      return ImGuiDir_Right;
    case vdl::GUI::DirectionType::eUp:
      return ImGuiDir_Up;
    case vdl::GUI::DirectionType::eDown:
      return ImGuiDir_Down;
    default: assert(false);
    }

    return ImGuiDir_None;
  }

  inline ImGuiDataType_ Cast(vdl::GUI::Detail::DataType _Type)
  {
    switch (_Type)
    {
    case vdl::GUI::Detail::DataType::eS8:
      return ImGuiDataType_S8;
    case vdl::GUI::Detail::DataType::eU8:
      return ImGuiDataType_U8;
    case vdl::GUI::Detail::DataType::eS16:
      return ImGuiDataType_S16;
    case vdl::GUI::Detail::DataType::eU16:
      return ImGuiDataType_U16;
    case vdl::GUI::Detail::DataType::eS32:
      return ImGuiDataType_S32;
    case vdl::GUI::Detail::DataType::eU32:
      return ImGuiDataType_U32;
    case vdl::GUI::Detail::DataType::eS64:
      return ImGuiDataType_S64;
    case vdl::GUI::Detail::DataType::eU64:
      return ImGuiDataType_U64;
    case vdl::GUI::Detail::DataType::eFloat:
      return ImGuiDataType_Float;
    case vdl::GUI::Detail::DataType::eDouble:
      return ImGuiDataType_Double;
    default: assert(false);
    }

    return ImGuiDataType_S8;
  }
}

namespace vdl::GUI
{
  bool Begin(const char* _Name, bool* _isOpen, const WindowFlags& _Flags)
  {
    return ImGui::Begin(_Name, _isOpen, _Flags);
  }

  void End()
  {
    ImGui::End();
  }

  //--------------------------------------------------

  bool BeginChild(const char* _StrID, const float2& _Size, bool _isBorder, const WindowFlags& _Flags)
  {
    return ImGui::BeginChild(_StrID, _Size, _isBorder, _Flags);
  }

  void EndChild()
  {
    ImGui::EndChild();
  }

  //--------------------------------------------------

  bool IsWindowAppearing()
  {
    return ImGui::IsWindowAppearing();
  }

  bool IsWindowCollapsed()
  {
    return ImGui::IsWindowCollapsed();
  }

  bool IsWindowDocked()
  {
    return ImGui::IsWindowDocked();
  }

  bool IsWindowFocused(const FocusedFlags& _Flags)
  {
    return ImGui::IsWindowFocused(_Flags);
  }

  bool IsWindowHovered(const HoveredFlags& _Flags)
  {
    return ImGui::IsWindowHovered(_Flags);
  }

  float2 GetWindowPos()
  {
    return ImGui::GetWindowPos();
  }

  float2 GetWindowSize()
  {
    return ImGui::GetWindowSize();
  }

  float GetWindowWidth()
  {
    return ImGui::GetWindowWidth();
  }

  float GetWindowHeight()
  {
    return ImGui::GetWindowHeight();
  }

  //--------------------------------------------------

  void SetNextWindowPos(const float2& _Pos, Condition _Condition, const float2& _Pivot)
  {
    ImGui::SetNextWindowPos(_Pos, Cast(_Condition), _Pivot);
  }

  void SetNextWindowSize(const float2& _Size, Condition _Condition)
  {
    ImGui::SetNextWindowSize(_Size, Cast(_Condition));
  }

  void SetNextWindowContentSize(const float2& _Size)
  {
    ImGui::SetNextWindowContentSize(_Size);
  }

  void SetNextWindowCollapsed(bool _isCollapsed, Condition _Condition)
  {
    ImGui::SetNextWindowCollapsed(_isCollapsed, Cast(_Condition));
  }

  void SetNextWindowFocus()
  {
    ImGui::SetNextWindowFocus();
  }

  void SetNextWindowBgAlpha(float _Alpha)
  {
    ImGui::SetNextWindowBgAlpha(_Alpha);
  }

  void SetWindowPos(const float2& _Pos, Condition _Condition)
  {
    ImGui::SetWindowPos(_Pos, Cast(_Condition));
  }

  void SetWindowSize(const float2& _Size, Condition _Condition)
  {
    ImGui::SetWindowSize(_Size, Cast(_Condition));
  }

  void SetWindowCollapsed(bool _isCollapsed, Condition _Condition)
  {
    ImGui::SetWindowCollapsed(_isCollapsed, Cast(_Condition));
  }

  void SetWindowFocus()
  {
    ImGui::SetWindowFocus();
  }

  void SetWindowFontScale(float _Scale)
  {
    ImGui::SetWindowFontScale(_Scale);
  }

  void SetWindowPos(const char* _Name, const float2& _Pos, Condition _Condition)
  {
    ImGui::SetWindowPos(_Name, _Pos, Cast(_Condition));
  }

  void SetWindowSize(const char* _Name, const float2& _Size, Condition _Condition)
  {
    ImGui::SetWindowSize(_Name, _Size, Cast(_Condition));
  }

  void SetWindowCollapsed(const char* _Name, bool _isCollapsed, Condition _Condition)
  {
    ImGui::SetWindowCollapsed(_Name, _isCollapsed, Cast(_Condition));
  }

  void SetWindowFocus(const char* _Name)
  {
    ImGui::SetWindowFocus(_Name);
  }

  //--------------------------------------------------

  float2 GetContentRegionMax()
  {
    return ImGui::GetContentRegionMax();
  }

  float2 GetContentRegionAvail()
  {
    return ImGui::GetContentRegionAvail();
  }

  float2 GetWindowContentRegionMin()
  {
    return ImGui::GetWindowContentRegionMin();
  }

  float2 GetWindowContentRegionMax()
  {
    return ImGui::GetWindowContentRegionMax();
  }

  float GetWindowContentRegionWidth()
  {
    return ImGui::GetWindowContentRegionWidth();
  }

  //--------------------------------------------------

  float GetScrollX()
  {
    return ImGui::GetScrollX();
  }

  float GetScrollY()
  {
    return ImGui::GetScrollY();
  }

  float GetScrollMaxX()
  {
    return ImGui::GetScrollMaxX();
  }

  float GetScrollMaxY()
  {
    return ImGui::GetScrollMaxY();
  }

  void SetScrollX(float _ScrollX)
  {
    ImGui::SetScrollX(_ScrollX);
  }

  void SetScrollY(float _ScrollY)
  {
    ImGui::SetScrollY(_ScrollY);
  }

  void SetScrollHereX(float _CenterRatioX)
  {
    ImGui::SetScrollHereX(_CenterRatioX);
  }

  void SetScrollHereY(float _CenterRatioY)
  {
    ImGui::SetScrollHereY(_CenterRatioY);
  }

  void SetScrollFromPosX(float _LocalX, float _CenterRatioX)
  {
    ImGui::SetScrollFromPosX(_LocalX, _CenterRatioX);
  }

  void SetScrollFromPosY(float _LocalY, float _CenterRatioY)
  {
    ImGui::SetScrollFromPosY(_LocalY, _CenterRatioY);
  }

  //--------------------------------------------------

  void PushItemWidth(float _ItemWidth)
  {
    ImGui::PushItemWidth(_ItemWidth);
  }

  void PopItemWidth()
  {
    ImGui::PopItemWidth();
  }

  void SetNextItemWidth(float _ItemWidth)
  {
    ImGui::SetNextItemWidth(_ItemWidth);
  }

  float CalcItemWidth()
  {
    return ImGui::CalcItemWidth();
  }

  void PushTextWrapPos(float _WrapLocalPosX)
  {
    ImGui::PushTextWrapPos(_WrapLocalPosX);
  }

  void PopTextWrapPos()
  {
    ImGui::PopTextWrapPos();
  }

  void PushAllowKeyboardFocus(bool _AllowKeyboardFocus)
  {
    ImGui::PushAllowKeyboardFocus(_AllowKeyboardFocus);
  }

  void PopAllowKeyboardFocus()
  {
    ImGui::PopAllowKeyboardFocus();
  }

  void PushButtonRepeat(bool _Repeat)
  {
    ImGui::PushButtonRepeat(_Repeat);
  }

  void PopButtonRepeat()
  {
    ImGui::PopButtonRepeat();
  }

  //--------------------------------------------------

  void Separator()
  {
    ImGui::Separator();
  }

  void SameLine(float _OffsetFromStartX, float _Spacing)
  {
    ImGui::SameLine(_OffsetFromStartX, _Spacing);
  }

  void NewLine()
  {
    ImGui::NewLine();
  }

  void Spacing()
  {
    ImGui::Spacing();
  }

  void Dummy(const float2& _Size)
  {
    ImGui::Dummy(_Size);
  }

  void Indent(float _IndentW)
  {
    ImGui::Indent(_IndentW);
  }

  void Unindent(float _IndentW)
  {
    ImGui::Unindent(_IndentW);
  }

  void BeginGroup()
  {
    ImGui::BeginGroup();
  }

  void EndGroup()
  {
    ImGui::EndGroup();
  }

  void AlignTextToFramePadding()
  {
    ImGui::AlignTextToFramePadding();
  }

  float GetTextLineHeight()
  {
    return ImGui::GetTextLineHeight();
  }

  float GetTextLineHeightWithSpacing()
  {
    return ImGui::GetTextLineHeightWithSpacing();
  }

  float GetFrameHeight()
  {
    return ImGui::GetFrameHeight();
  }

  float GetFrameHeightWithSpacing()
  {
    return ImGui::GetFrameHeightWithSpacing();
  }

  //--------------------------------------------------

  void TextUnformatted(const char* _Text, const char* _TextEnd)
  {
    ImGui::TextUnformatted(_Text, _TextEnd);
  }

  void Text(const char* _Format, ...)
  {
    va_list Args;
    va_start(Args, _Format);
    ImGui::TextV(_Format, Args);
    va_end(Args);
  }

  void TextColored(const Color4F& _Color, const char* _Format, ...)
  {
    va_list Args;
    va_start(Args, _Format);
    ImGui::TextColoredV(_Color, _Format, Args);
    va_end(Args);
  }

  void TextDisabled(const char* _Format, ...)
  {
    va_list Args;
    va_start(Args, _Format);
    ImGui::TextDisabledV(_Format, Args);
    va_end(Args);
  }

  void TextWrapped(const char* _Format, ...)
  {
    va_list Args;
    va_start(Args, _Format);
    ImGui::TextWrapped(_Format, Args);
    va_end(Args);
  }

  void LabelText(const char* _Label, const char* _Format, ...)
  {
    va_list Args;
    va_start(Args, _Format);
    ImGui::LabelTextV(_Label, _Format, Args);
    va_end(Args);
  }

  void BulletText(const char* _Format, ...)
  {
    va_list Args;
    va_start(Args, _Format);
    ImGui::BulletTextV(_Format, Args);
    va_end(Args);
  }

  //--------------------------------------------------

  bool Button(const char* _Label, const float2& _Size)
  {
    return ImGui::Button(_Label, _Size);
  }

  bool SmallButton(const char* _Label)
  {
    return ImGui::SmallButton(_Label);
  }

  bool InvisibleButton(const char* _StrID, const float2& _Size)
  {
    return ImGui::InvisibleButton(_StrID, _Size);
  }

  bool ArrowButton(const char* _StrID, DirectionType _Type)
  {
    return ImGui::ArrowButton(_StrID, Cast(_Type));
  }

  void Image(const Texture& _Texture, const float2& _Size, const float2& _UV0, const float2& _UV1, const Color4F& _TintColor, const Color4F& _BorderColor)
  {
    ImGui::Image(_Texture, _Size, _UV0, _UV1, _TintColor, _BorderColor);
  }

  bool ImageButton(const Texture& _Texture, const float2& _Size, const float2& _UV0, const float2& _UV1, int _FramePadding, const Color4F& _BackgroundColor, const Color4F& _TintColor)
  {
    return ImGui::ImageButton(_Texture, _Size, _UV0, _UV1, _FramePadding, _BackgroundColor, _TintColor);
  }

  bool Checkbox(const char* _Label, bool* _Flag)
  {
    return ImGui::Checkbox(_Label, _Flag);
  }

  bool CheckboxFlags(const char* _Label, uint* _Flags, uint _FlagsValue)
  {
    return ImGui::CheckboxFlags(_Label, _Flags, _FlagsValue);
  }

  bool RadioButton(const char* _Label, bool _Active)
  {
    return ImGui::RadioButton(_Label, _Active);
  }

  void ProgressBar(float _Fraction, const float2& _SizeArg, const char* _Overlay)
  {
    ImGui::ProgressBar(_Fraction, _SizeArg, _Overlay);
  }

  void Bullet()
  {
    ImGui::Bullet();
  }

  //--------------------------------------------------

  bool BeginCombo(const char* _Label, const char* _PreviewValue, const ComboFlags& _Flags)
  {
    return ImGui::BeginCombo(_Label, _PreviewValue, _Flags);
  }

  void EndCombo()
  {
    ImGui::EndCombo();
  }

  bool Combo(const char* _Label, int* _pCurrentItem, const char* const _Items[], int _ItemNum, int _PopupMaxHeightInItems)
  {
    return ImGui::Combo(_Label, _pCurrentItem, _Items, _ItemNum, _PopupMaxHeightInItems);
  }

  bool Combo(const char* _Label, int* _pCurrentItem, const char* _ItemsSeparatedByZeros, int _PopupMaxHeightInItems)
  {
    return ImGui::Combo(_Label, _pCurrentItem, _ItemsSeparatedByZeros, _PopupMaxHeightInItems);
  }

  bool Combo(const char* _Label, int* _pCurrentItem, bool (*_ItemsGetter)(void* _pData, int _Index, const char** _OutText), void* _pData, int _ItemNum, int _PopupMaxHeightInItems)
  {
    return ImGui::Combo(_Label, _pCurrentItem, _ItemsGetter, _pData, _ItemNum, _PopupMaxHeightInItems);
  }

  //--------------------------------------------------

  bool InputText(const char* _Label, char* _Buffer, size_t _BufferSize, const InputTextFlags& _Flags)
  {
    return ImGui::InputText(_Label, _Buffer, _BufferSize, _Flags);
  }

  bool InputTextMultiline(const char* _Label, char* _Buffer, size_t _BufferSize, const float2& _Size, const InputTextFlags& _Flags)
  {
    return ImGui::InputTextMultiline(_Label, _Buffer, _BufferSize, _Size, _Flags);
  }

  bool InputTextWithHint(const char* _Label, const char* _Hint, char* _Buffer, size_t _BufferSize, const InputTextFlags& _Flags)
  {
    return ImGui::InputTextWithHint(_Label, _Hint, _Buffer, _BufferSize, _Flags);
  }

  //--------------------------------------------------

  bool ColorEdit(const char* _Label, Color3F* _pColor, const ColorEditFlags& _Flags)
  {
    return ImGui::ColorEdit3(_Label, &_pColor->Red, _Flags);
  }

  bool ColorEdit(const char* _Label, Color4F* _pColor, const ColorEditFlags& _Flags)
  {
    return ImGui::ColorEdit4(_Label, &_pColor->Red, _Flags);
  }

  bool ColorPicker(const char* _Label, Color3F* _pColor, const ColorEditFlags& _Flags)
  {
    return ImGui::ColorPicker3(_Label, &_pColor->Red, _Flags);
  }

  bool ColorPicker(const char* _Label, Color4F* _pColor, const ColorEditFlags& _Flags, const Color4F* _pRefColor)
  {
    return ImGui::ColorPicker4(_Label, &_pColor->Red, _Flags, &_pRefColor->Red);
  }

  bool ColorButton(const char* _DescID, const Color4F& _Color, const ColorEditFlags& _Flags, const float2& _Size)
  {
    return ImGui::ColorButton(_DescID, _Color, _Flags, _Size);
  }

  void SetColorEditOptions(const ColorEditFlags& _Flags)
  {
    ImGui::SetColorEditOptions(_Flags);
  }

  //--------------------------------------------------

  bool TreeNode(const char* _Label)
  {
    return ImGui::TreeNode(_Label);
  }

  bool TreeNode(const char* _StrID, const char* _Format, ...)
  {
    va_list Args;
    va_start(Args, _Format);
    const bool Result = ImGui::TreeNodeV(_StrID, _Format, Args);
    va_end(Args);

    return Result;
  }

  bool TreeNodeEx(const char* _Label, const TreeNodeFlags& _Flags)
  {
    return ImGui::TreeNodeEx(_Label, _Flags);
  }

  bool TreeNodeEx(const char* _StrID, const TreeNodeFlags& _Flags, const char* _Format, ...)
  {
    va_list Args;
    va_start(Args, _Format);
    const bool Result = ImGui::TreeNodeExV(_StrID, _Flags, _Format, Args);
    va_end(Args);

    return Result;
  }

  void TreePush(const char* _StrID)
  {
    ImGui::TreePush(_StrID);
  }

  void TreePop()
  {
    ImGui::TreePop();
  }

  float GetTreeNodeToLabelSpacing()
  {
    return ImGui::GetTreeNodeToLabelSpacing();
  }

  bool CollapsingHeader(const char* _Label, const TreeNodeFlags& _Flags)
  {
    return ImGui::CollapsingHeader(_Label, _Flags);
  }

  bool CollapsingHeader(const char* _Label, bool* _pOpen, const TreeNodeFlags& _Flags)
  {
    return ImGui::CollapsingHeader(_Label, _pOpen, _Flags);
  }

  void SetNextItemOpen(bool _isOpen, Condition _Condition)
  {
    ImGui::SetNextItemOpen(_isOpen, Cast(_Condition));
  }

  //--------------------------------------------------

  bool Selectable(const char* _Label, bool _Selected, const SelectableFlags& _Flags, const float2& _Size)
  {
    return ImGui::Selectable(_Label, _Selected, _Flags, _Size);
  }

  bool Selectable(const char* _Label, bool* _pSelected, const SelectableFlags& _Flags, const float2& _Size)
  {
    return ImGui::Selectable(_Label, _pSelected, _Flags, _Size);
  }

  //--------------------------------------------------

  bool ListBox(const char* _Label, int* _pCurrentItem, const char* const _Items[], int _ItemNum, int _HeightInItems)
  {
    return ImGui::ListBox(_Label, _pCurrentItem, _Items, _ItemNum, _HeightInItems);
  }

  bool ListBox(const char* _Label, int* _pCurrentItem, bool (*_ItemsGetter)(void* _pData, int _Index, const char** _OutText), void* _pData, int _ItemNum, int _HeightInItems)
  {
    return ImGui::ListBox(_Label, _pCurrentItem, _ItemsGetter, _pData, _ItemNum, _HeightInItems);
  }

  bool ListBoxHeader(const char* _Label, const float2& _Size)
  {
    return ImGui::ListBoxHeader(_Label, _Size);
  }

  bool ListBoxHeader(const char* _Label, int _ItemNum, int _HeightInItems)
  {
    return ImGui::ListBoxHeader(_Label, _ItemNum, _HeightInItems);
  }

  void ListBoxFooter()
  {
    ImGui::ListBoxFooter();
  }

  //--------------------------------------------------

  void PlotLines(const char* _Label, const float* _Values, int _ValueNum, int _ValuesOffset, const char* _OverlayText, float _ScaleMin, float _ScaleMax, const float2& _GraphSize)
  {
    ImGui::PlotLines(_Label, _Values, _ValueNum, _ValuesOffset, _OverlayText, _ScaleMin, _ScaleMax, _GraphSize);
  }

  void PlotLines(const char* _Label, float(*_ValuesGetter)(void* _pData, int _Index), void* _pData, int _ValueNum, int _ValuesOffset, const char* _OverlayText, float _ScaleMin, float _ScaleMax, const float2& _GraphSize)
  {
    ImGui::PlotLines(_Label, _ValuesGetter, _pData, _ValueNum, _ValuesOffset, _OverlayText, _ScaleMin, _ScaleMax, _GraphSize);
  }

  void PlotHistogram(const char* _Label, const float* _Values, int _ValueNum, int _ValuesOffset, const char* _OverlayText, float _ScaleMin, float _ScaleMax, const float2& _GraphSize)
  {
    ImGui::PlotHistogram(_Label, _Values, _ValueNum, _ValuesOffset, _OverlayText, _ScaleMin, _ScaleMax, _GraphSize);
  }

  void PlotHistogram(const char* _Label, float(*_ValuesGetter)(void* _pData, int _Index), void* _pData, int _ValueNum, int _ValuesOffset, const char* _OverlayText, float _ScaleMin, float _ScaleMax, const float2& _GraphSize)
  {
    ImGui::PlotHistogram(_Label, _ValuesGetter, _pData, _ValueNum, _ValuesOffset, _OverlayText, _ScaleMin, _ScaleMax, _GraphSize);
  }

  //--------------------------------------------------

  bool BeginMenuBar()
  {
    return ImGui::BeginMenuBar();
  }

  void EndMenuBar()
  {
    ImGui::EndMenuBar();
  }

  bool BeginMainMenuBar()
  {
    return ImGui::BeginMainMenuBar();
  }

  void EndMainMenuBar()
  {
    ImGui::EndMainMenuBar();
  }

  bool BeginMenu(const char* _Label, bool _Enabled)
  {
    return ImGui::BeginMenu(_Label, _Enabled);
  }

  void EndMenu()
  {
    ImGui::EndMenu();
  }

  bool MenuItem(const char* _Label, const char* _Shortcut, bool _Selected, bool _Enabled)
  {
    return ImGui::MenuItem(_Label, _Shortcut, _Selected, _Enabled);
  }

  bool MenuItem(const char* _Label, const char* _Shortcut, bool* _pSelected, bool _Enabled)
  {
    return ImGui::MenuItem(_Label, _Shortcut, _pSelected, _Enabled);
  }

  //--------------------------------------------------

  void BeginTooltip()
  {
    ImGui::BeginTooltip();
  }

  void EndTooltip()
  {
    ImGui::EndTooltip();
  }

  void SetTooltip(const char* _Format, ...)
  {
    va_list Args;
    va_start(Args, _Format);
    ImGui::SetTooltipV(_Format, Args);
    va_end(Args);
  }

  //--------------------------------------------------

  void OpenPopup(const char* _StrID)
  {
    ImGui::OpenPopup(_StrID);
  }

  bool BeginPopup(const char* _StrID, const WindowFlags& _Flags)
  {
    return ImGui::BeginPopup(_StrID, _Flags);
  }

  bool BeginPopupContextItem(const char* _StrID)
  {
    return ImGui::BeginPopupContextItem(_StrID);
  }

  bool BeginPopupContextWindow(const char* _StrID, bool _AlsoOverItems)
  {
    return ImGui::BeginPopupContextWindow(_StrID, _AlsoOverItems);
  }

  bool BeginPopupContextVoid(const char* _StrID)
  {
    return ImGui::BeginPopupContextVoid(_StrID);
  }

  bool BeginPopupModal(const char* _Name, bool* _pOpen, const WindowFlags& _Flags)
  {
    return ImGui::BeginPopupModal(_Name, _pOpen, _Flags);
  }

  void EndPopup()
  {
    ImGui::EndPopup();
  }

  //--------------------------------------------------

  void Columns(int _Count, const char* _StrID, bool _Border)
  {
    ImGui::Columns(_Count, _StrID, _Border);
  }

  void NextColumn()
  {
    ImGui::NextColumn();
  }

  int GetColumnIndex()
  {
    return ImGui::GetColumnIndex();
  }

  float GetColumnWidth(int _ColumnIndex)
  {
    return ImGui::GetColumnWidth(_ColumnIndex);
  }

  void SetColumnWidth(int _ColumnIndex, float _Width)
  {
    ImGui::SetColumnWidth(_ColumnIndex, _Width);
  }

  float GetColumnOffset(int _ColumnIndex)
  {
    return ImGui::GetColumnOffset(_ColumnIndex);
  }

  void SetColumnOffset(int _ColumnIndex, float _OffsetX)
  {
    ImGui::SetColumnOffset(_ColumnIndex, _OffsetX);
  }

  int GetColumnsCount()
  {
    return ImGui::GetColumnsCount();
  }

  //--------------------------------------------------

  bool BeginTabBar(const char* _StrID, const TabBarFlags& _Flags)
  {
    return ImGui::BeginTabBar(_StrID, _Flags);
  }

  void EndTabBar()
  {
    ImGui::EndTabBar();
  }

  bool BeginTabItem(const char* _Label, bool* _pOpen, const TabItemFlags& _Flags)
  {
    return ImGui::BeginTabItem(_Label, _pOpen, _Flags);
  }

  void EndTabItem()
  {
    ImGui::EndTabItem();
  }

  void SetTabItemClosed(const char* _TabOrDockedWindowLabel)
  {
    ImGui::SetTabItemClosed(_TabOrDockedWindowLabel);
  }

  //--------------------------------------------------

  void PushClipRect(const float2& _MinClipRect, const float2& _MaxClipRect, bool _IntersectWithCurrentClipRect)
  {
    ImGui::PushClipRect(_MinClipRect, _MaxClipRect, _IntersectWithCurrentClipRect);
  }

  void PopClipRect()
  {
    ImGui::PopClipRect();
  }

  //--------------------------------------------------

  void SetItemDefaultFocus()
  {
    ImGui::SetItemDefaultFocus();
  }

  void SetKeyboardFocusHere(int _Offset)
  {
    ImGui::SetKeyboardFocusHere(_Offset);
  }

  //--------------------------------------------------

  bool IsItemHovered(const HoveredFlags& _Flags)
  {
    return ImGui::IsItemHovered(_Flags);
  }

  bool IsItemActive()
  {
    return ImGui::IsItemActive();
  }

  bool IsItemFocused()
  {
    return ImGui::IsItemFocused();
  }

  bool IsItemClicked()
  {
    return ImGui::IsItemClicked();
  }

  bool IsItemVisible()
  {
    return ImGui::IsItemVisible();
  }

  bool IsItemEdited()
  {
    return ImGui::IsItemEdited();
  }

  bool IsItemActivated()
  {
    return ImGui::IsItemActivated();
  }

  bool IsItemDeactivated()
  {
    return ImGui::IsItemDeactivated();
  }

  bool IsItemDeactivatedAfterEdit()
  {
    return ImGui::IsItemDeactivatedAfterEdit();
  }

  bool IsItemToggledOpen()
  {
    return ImGui::IsItemToggledOpen();
  }

  bool IsAnyItemHovered()
  {
    return ImGui::IsAnyItemHovered();
  }

  bool IsAnyItemActive()
  {
    return ImGui::IsAnyItemActive();
  }

  bool IsAnyItemFocused()
  {
    return ImGui::IsAnyItemFocused();
  }

  float2 GetItemRectMin()
  {
    return ImGui::GetItemRectMin();
  }

  float2 GetItemRectMax()
  {
    return ImGui::GetItemRectMax();
  }

  float2 GetItemRectSize()
  {
    return ImGui::GetItemRectSize();
  }

  void SetItemAllowOverlap()
  {
    ImGui::SetItemAllowOverlap();
  }

  //--------------------------------------------------

  bool IsRectVisible(const float2& _Size)
  {
    return ImGui::IsRectVisible(_Size);
  }

  bool IsRectVisible(const float2& _MinRect, const float2& _MaxRect)
  {
    return ImGui::IsRectVisible(_MinRect, _MaxRect);
  }

  float2 CalcTextSize(const char* _Text, const char* _TextEnd, bool _HideTextAfterDoubleHash, float _WrapWidth)
  {
    return ImGui::CalcTextSize(_Text, _TextEnd, _HideTextAfterDoubleHash, _WrapWidth);
  }

  void CalcListClipping(int _ItemNum, float _ItemHeight, int* _OutItemDisplayStart, int* _OutItemDisplayEnd)
  {
    ImGui::CalcListClipping(_ItemNum, _ItemHeight, _OutItemDisplayStart, _OutItemDisplayEnd);
  }

  //--------------------------------------------------

  const char* GetClipboardText()
  {
    return ImGui::GetClipboardText();
  }

  void SetClipboardText(const char* _Text)
  {
    ImGui::SetClipboardText(_Text);
  }

  //--------------------------------------------------

  namespace Detail
  {
    bool DragScalar(const char* _Label, DataType _Type, void* _pData, float _Speed, const void* _pMin, const void* _pMax, const char* _Format, const SliderFlags& _Flags)
    {
      return ImGui::DragScalar(_Label, Cast(_Type), _pData, _Speed, _pMin, _pMax, _Format, _Flags);
    }

    bool DragScalarN(const char* _Label, DataType _Type, void* _pData, int _Components, float _Speed, const void* _pMin, const void* _pMax, const char* _Format, const SliderFlags& _Flags)
    {
      return ImGui::DragScalarN(_Label, Cast(_Type), _pData, _Components, _Speed, _pMin, _pMax, _Format, _Flags);
    }

    bool SliderScalar(const char* _Label, DataType _Type, void* _pData, const void* _pMin, const void* _pMax, const char* _Format, const SliderFlags& _Flags)
    {
      return ImGui::SliderScalar(_Label, Cast(_Type), _pData, _pMin, _pMax, _Format, _Flags);
    }

    bool SliderScalarN(const char* _Label, DataType _Type, void* _pData, int _Components, const void* _pMin, const void* _pMax, const char* _Format, const SliderFlags& _Flags)
    {
      return ImGui::SliderScalarN(_Label, Cast(_Type), _pData, _Components, _pMin, _pMax, _Format, _Flags);
    }

    bool InputScalar(const char* _Label, DataType _Type, void* _pData, const void* _pStep, const void* _pStepFast, const char* _Format, const InputTextFlags& _Flags)
    {
      return ImGui::InputScalar(_Label, Cast(_Type), _pData, _pStep, _pStepFast, _Format, _Flags);
    }

    bool InputScalarN(const char* _Label, DataType _Type, void* _pData, int _Components, const void* _pStep, const void* _pStepFast, const char* _Format, const InputTextFlags& _Flags)
    {
      return ImGui::InputScalarN(_Label, Cast(_Type), _pData, _Components, _pStep, _pStepFast, _Format, _Flags);
    }
  }
}
