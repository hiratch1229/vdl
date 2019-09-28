#pragma once
#include "../IModelManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

class IDevice;

class CModelManager : public IModelManager
{
  IDevice* pDevice_;
  ReferenceCounts<Mesh> Meshes_;
public:
  CModelManager() = default;

  void Initialize()override;

  vdl::ID Load(const vdl::StaticMeshData& _MeshData)override;

  vdl::ID Load(vdl::StaticMeshData&& _MeshData)override;

  vdl::ID Load(const vdl::SkinnedMeshData& _MeshData)override;

  vdl::ID Load(vdl::SkinnedMeshData&& _MeshData)override;

  std::vector<vdl::SkinnedMesh> Load(const char* _FilePath, bool _isSerialize)override;

  void AddRef(const vdl::ID& _ID)override { Meshes_.Get(_ID).AddRef(); }

  void Release(const vdl::ID& _ID)override { Meshes_.Get(_ID).Release(); }

  Mesh* GetMesh(const vdl::ID& _ID)override { return Meshes_.Get(_ID).Get(); }
};
