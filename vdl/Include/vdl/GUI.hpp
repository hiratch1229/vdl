#pragma once
#include "Type2.hpp"
#include "Type3.hpp"
#include "Type4.hpp"
#include "Color.hpp"

#include "../ThirdParty/ImGui/imgui.h"

namespace ImGui
{
  IMGUI_API inline bool DragFloat2(const char* label, vdl::float2* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f)
  {
    return DragFloat2(label, &v->x, v_speed, v_min, v_max, format, power);
  }

  IMGUI_API inline bool DragFloat3(const char* label, vdl::float3* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f)
  {
    return DragFloat3(label, &v->x, v_speed, v_min, v_max, format, power);
  }

  IMGUI_API inline bool DragFloat4(const char* label, vdl::float4* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f)
  {
    return DragFloat4(label, &v->x, v_speed, v_min, v_max, format, power);
  }
  IMGUI_API inline bool DragInt2(const char* label, vdl::int2* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d")
  {
    return DragInt2(label, &v->x, v_speed, v_min, v_max, format);
  }

  IMGUI_API inline bool DragInt3(const char* label, vdl::int3* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d")
  {
    return DragInt3(label, &v->x, v_speed, v_min, v_max, format);
  }

  IMGUI_API inline bool DragInt4(const char* label, vdl::int4* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d")
  {
    return DragInt4(label, &v->x, v_speed, v_min, v_max, format);
  }

  IMGUI_API inline bool SliderFloat2(const char* label, vdl::float2* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f)
  {
    return SliderFloat2(label, &v->x, v_min, v_max, format, power);
  }

  IMGUI_API inline bool SliderFloat3(const char* label, vdl::float3* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f)
  {
    return SliderFloat3(label, &v->x, v_min, v_max, format, power);
  }

  IMGUI_API inline bool SliderFloat4(const char* label, vdl::float4* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f)
  {
    return SliderFloat4(label, &v->x, v_min, v_max, format, power);
  }

  IMGUI_API inline bool SliderInt2(const char* label, vdl::int2* v, int v_min, int v_max, const char* format = "%d")
  {
    return SliderInt2(label, &v->x, v_min, v_max, format);
  }

  IMGUI_API inline bool SliderInt3(const char* label, vdl::int3* v, int v_min, int v_max, const char* format = "%d")
  {
    return SliderInt3(label, &v->x, v_min, v_max, format);
  }

  IMGUI_API inline bool SliderInt4(const char* label, vdl::int4* v, int v_min, int v_max, const char* format = "%d")
  {
    return SliderInt4(label, &v->x, v_min, v_max, format);
  }

  IMGUI_API inline bool InputFloat2(const char* label, vdl::float2* v, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
  {
    return InputFloat2(label, &v->x, format, flags);
  }

  IMGUI_API inline bool InputFloat3(const char* label, vdl::float3* v, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
  {
    return InputFloat3(label, &v->x, format, flags);
  }

  IMGUI_API inline bool InputFloat4(const char* label, vdl::float4* v, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
  {
    return InputFloat4(label, &v->x, format, flags);
  }

  IMGUI_API inline bool InputInt2(const char* label, vdl::int2* v, ImGuiInputTextFlags flags = 0)
  {
    return InputInt2(label, &v->x, flags);
  }

  IMGUI_API inline bool InputInt3(const char* label, vdl::int3* v, ImGuiInputTextFlags flags = 0)
  {
    return InputInt3(label, &v->x, flags);
  }

  IMGUI_API inline bool InputInt4(const char* label, vdl::int4* v, ImGuiInputTextFlags flags = 0)
  {
    return InputInt4(label, &v->x, flags);
  }

  IMGUI_API inline bool ColorEdit3(const char* label, vdl::ColorF* col, ImGuiColorEditFlags flags = 0)
  {
    return ColorEdit3(label, &col->Red, flags);
  }

  IMGUI_API inline bool ColorEdit4(const char* label, vdl::ColorF* col, ImGuiColorEditFlags flags = 0)
  {
    return ColorEdit4(label, &col->Red, flags);
  }

  IMGUI_API inline bool ColorPicker3(const char* label, vdl::ColorF* col, ImGuiColorEditFlags flags = 0)
  {
    return ColorPicker3(label, &col->Red, flags);
  }

  IMGUI_API inline bool ColorPicker4(const char* label, vdl::ColorF* col, ImGuiColorEditFlags flags = 0, const float* ref_col = NULL)
  {
    return ColorPicker4(label, &col->Red, flags, ref_col);
  }
}
