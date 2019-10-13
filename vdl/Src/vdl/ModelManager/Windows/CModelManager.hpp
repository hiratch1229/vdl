#pragma once
#include "../IModelManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

class IDevice;

class CModelManager : public IModelManager
{
  ReferenceCounts<Mesh> Meshes_;
public:
  CModelManager() = default;

  void Initialize()override;

  vdl::ID Load(const vdl::MeshData& _MeshData)override;

  vdl::ID Load(vdl::MeshData&& _MeshData)override;

  std::vector<vdl::Mesh> Load(const char* _FilePath, bool _isSerialize)override;

  void AddRef(const vdl::ID& _ID)override { Meshes_.Get(_ID).AddRef(); }

  void Release(const vdl::ID& _ID)override { Meshes_.Get(_ID).Release(); }

  Mesh* GetMesh(const vdl::ID& _ID)override { return Meshes_.Get(_ID).Get(); }
};
