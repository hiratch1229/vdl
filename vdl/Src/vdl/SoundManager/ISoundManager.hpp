#pragma once
#include <vdl/ID.hpp>

class ISoundManager
{
public:
  static ISoundManager* Create();

  ISoundManager() = default;

  virtual ~ISoundManager() = default;

  virtual void Initialize() = 0;

  virtual ID Load(const char* _FilePath) = 0;

  virtual void AddRef(const ID& _ID) = 0;

  virtual void Release(const ID& _ID) = 0;
};
