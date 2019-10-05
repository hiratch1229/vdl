#pragma once
#include <vdl/Fwd.hpp>

class ISoundManager
{
public:
  static ISoundManager* Create();

  ISoundManager() = default;

  virtual ~ISoundManager() = default;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual vdl::ID Load(const char* _FilePath) = 0;

  virtual void AddRef(const vdl::ID& _ID) = 0;

  virtual void Release(const vdl::ID& _ID) = 0;
};
