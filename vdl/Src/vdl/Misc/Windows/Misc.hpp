#pragma once
#include <vdl/pch/Windows/pch.hpp>

#include <string>

//--------------------------------------------------
//  ÉGÉâÅ[
//--------------------------------------------------
#if defined( DEBUG ) || defined( _DEBUG )
#define _ASSERT_EXPR_A(expr, msg) \
	(void)((!!(expr)) || \
	(1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", msg)) || \
	(_CrtDbgBreak(), 0))

inline LPWSTR hResultTrace(HRESULT hr)
{
  LPWSTR Msg;
  FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&Msg), 0, NULL);
  return Msg;
}
#else
#define  _ASSERT_EXPR_A(expr, expr_str) ((void)0)
#endif

inline std::string GetFileFormat(const char* _FileName)
{
  const size_t Index = std::string(_FileName).rfind('.');
  _ASSERT_EXPR(Index != -1, ".Ç™å©Ç¬Ç©ÇËÇ‹ÇπÇÒÅB");

  return &_FileName[Index + 1];
}
