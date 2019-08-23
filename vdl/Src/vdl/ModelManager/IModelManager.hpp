#pragma once
#include <vdl/ID.hpp>

#include "Mesh.hpp"

class IModelManager
{
public:
  static IModelManager* Create();

  IModelManager() = default;

  virtual ~IModelManager() = default;

  virtual void Initialize() = 0;

  virtual ID Load(const char* _FilePath) = 0;

  virtual Mesh* GetMesh(const ID& _ID) = 0;
};
