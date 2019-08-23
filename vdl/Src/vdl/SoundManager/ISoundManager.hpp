#pragma once
#include <vdl/ID.hpp>

class ISoundManager
{
public:
  static ISoundManager* Create();

  ISoundManager() = default;

  virtual ~ISoundManager() = default;

  virtual ID Load(const char* _FilePath) = 0;
};
