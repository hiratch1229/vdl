#pragma once
#include <crtdbg.h>

#include <string>

#if defined( DEBUG ) || defined( _DEBUG )
#define _ASSERT_EXPR_A(expr, msg) \
	(void)((!!(expr)) || \
	(1 != ::_CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", msg)) || \
	(_CrtDbgBreak(), 0))
#else
#define  _ASSERT_EXPR_A(expr, expr_str) ((void)0)
#endif

inline std::string GetFileFormat(const char* _FileName)
{
  const size_t Index = std::string(_FileName).rfind('.');
  _ASSERT_EXPR(Index != -1, ".Ç™å©Ç¬Ç©ÇËÇ‹ÇπÇÒÅB");

  return &_FileName[Index + 1];
}
