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

  [[nodiscard]] virtual vdl::ID Load(const vdl::Vertices& _Vertices, const vdl::Indices& _Indices, const vdl::MeshData& _MeshData) = 0;

  [[nodiscard]] virtual std::vector<vdl::Mesh> Load(const char* _FilePath, bool _isSerialize) = 0;

  virtual void AddRef(const vdl::ID& _ID) = 0;

  virtual void Release(const vdl::ID& _ID) = 0;

  [[nodiscard]] virtual Mesh* GetMesh(const vdl::ID& _ID) = 0;
};
