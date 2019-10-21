#pragma once
#include <vdl/pch/Windows/pch.hpp>

#include "../Misc.hpp"

#include <string>

#if defined( DEBUG ) || defined( _DEBUG )
inline LPWSTR hResultTrace(HRESULT hr)
{
  LPWSTR Msg;
  FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&Msg), 0, NULL);
  return Msg;
}
#endif
