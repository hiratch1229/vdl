#pragma once
#include <vdl/pch/Windows/pch.hpp>

#include "../Misc.hpp"

#if defined( DEBUG ) || defined( _DEBUG )
inline LPSTR hResultTrace(HRESULT hr)
{
  LPSTR Msg = {};
  ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
    nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),reinterpret_cast<LPSTR>(& Msg), 0, nullptr);
  
  return Msg;
}
#define ERROR_CHECK(hr) _ASSERT_EXPR_A(SUCCEEDED(hr), ::hResultTrace(hr));
#else
#define ERROR_CHECK(hr) ((void)0)
#endif
