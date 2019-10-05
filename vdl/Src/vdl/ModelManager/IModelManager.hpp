#pragma once
#include <vdl/Fwd.hpp>

#include "Mesh.hpp"

class IModelManager
{
public:
  static IModelManager* Create();

  IModelManager() = default;

  virtual ~IModelManager() = default;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual vdl::ID Load(const vdl::StaticMeshData& _MeshData) = 0;

  [[nodiscard]] virtual vdl::ID Load(vdl::StaticMeshData&& _MeshData) = 0;

  [[nodiscard]] virtual vdl::ID Load(const vdl::SkinnedMeshData& _MeshData) = 0;

  [[nodiscard]] virtual vdl::ID Load(vdl::SkinnedMeshData&& _MeshData) = 0;

  [[nodiscard]] virtual std::vector<vdl::SkinnedMesh> Load(const char* _FilePath, bool _isSerialize) = 0;

  virtual void AddRef(const vdl::ID& _ID) = 0;

  virtual void Release(const vdl::ID& _ID) = 0;

  [[nodiscard]] virtual Mesh* GetMesh(const vdl::ID& _ID) = 0;
};
