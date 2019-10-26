#pragma once
#include <vdl/Image.hpp>

#include <string>

class TextureLoader
{
public:
  static bool CheckSupportFormat(const std::string& _FileFormat);
public:
  TextureLoader() = default;

  ~TextureLoader() = default;

  vdl::Image LoadFromFile(const char* _FilePath)const;
};
