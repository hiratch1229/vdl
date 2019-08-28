#pragma once
#include "../IModelManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

class CModelManager : public IModelManager
{
  IDevice* pDevice_;
private:
  ReferenceCounts<Mesh> Meshes_;
public:
  CModelManager() = default;

  void Initialize(IDevice* _pDevice)override;

  ID Load(const vdl::SkinnedMeshData& _MeshData)override;

  std::vector<vdl::SkinnedMesh> Load(const char* _FilePath, bool _isSerialize)override;

  Mesh* GetMesh(const ID& _ID)override { return Meshes_.Get(_ID).Get(); }
};
