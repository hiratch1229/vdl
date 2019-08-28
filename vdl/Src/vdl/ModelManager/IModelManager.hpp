#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/ID.hpp>

#include "Mesh.hpp"

class IDevice;

class IModelManager
{
public:
  static IModelManager* Create();

  IModelManager() = default;

  virtual ~IModelManager() = default;

  virtual void Initialize(IDevice* _pDevice) = 0;

  virtual ID Load(const vdl::SkinnedMeshData& _MeshData) = 0;

  virtual std::vector<vdl::SkinnedMesh> Load(const char* _FilePath, bool _isSerialize) = 0;

  virtual Mesh* GetMesh(const ID& _ID) = 0;
};
