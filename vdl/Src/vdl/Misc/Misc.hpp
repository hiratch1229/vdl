#pragma once
#include <crtdbg.h>

#include <string>
#include <filesystem>

#if defined _DEBUG
#define _ASSERT_EXPR_A(expr, msg) \
	(void)((!!(expr)) || \
	(1 != ::_CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", msg)) || \
	(_CrtDbgBreak(), 0))
#else
#define _ASSERT_EXPR_A(expr, expr_str) ((void)0)
#endif

inline std::string GetFileFormat(const char* _FileName)
{
  const size_t Index = std::string(_FileName).rfind('.');
  _ASSERT_EXPR(Index != -1, std::string(std::string(_FileName) + ".が見つかりません。").c_str());

  return &_FileName[Index + 1];
}

//  元ファイルの更新日時が新しければtrue
inline bool isFileUpdate(const std::filesystem::path& _OriginalFilePath, const std::filesystem::path& _SerializeFilePath)
{
  _ASSERT_EXPR(std::filesystem::exists(_OriginalFilePath), std::string(_OriginalFilePath.string() + ".が見つかりません。").c_str());
  _ASSERT_EXPR(std::filesystem::exists(_SerializeFilePath), std::string(_SerializeFilePath.string() + ".が見つかりません。").c_str());

  return std::filesystem::last_write_time(_SerializeFilePath) < std::filesystem::last_write_time(_OriginalFilePath);
}
