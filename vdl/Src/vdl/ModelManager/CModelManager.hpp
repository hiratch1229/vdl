#pragma once
#include "IModelManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

class IDevice;

class CModelManager : public IModelManager
{
  struct VertexBufferData
  {
    VertexBuffer VertexBuffer;
    vdl::VertexSkinnedMeshs Vertices;
  };
  struct IndexBufferData
  {
    IndexBuffer IndexBuffer;
    vdl::Indices Indices;
  };
private:
  ReferenceCounts<Mesh> Meshes_;
  std::vector<ReferenceCount<VertexBufferData>> VertexBufferDatas_;
  std::vector<ReferenceCount<IndexBufferData>> IndexBufferDatas_;
public:
  CModelManager() = default;

  PlatformType GetPlatform()const final { return PlatformType::eCommon; }

  void Initialize()override {}

  vdl::ID Load(const vdl::VertexStaticMeshs& _Vertices, const vdl::Indices& _Indices, const vdl::StaticMeshData& _StaticMeshData)override;

  vdl::ID Load(const vdl::VertexSkinnedMeshs& _Vertices, const vdl::Indices& _Indices, const vdl::SkinnedMeshData& _SkinnedMeshData)override;

  std::vector<vdl::SkinnedMesh> Load(const char* _FilePath)override;

  void AddRef(vdl::ID _ID)override
  {
    ReferenceCount<Mesh>& Mesh = Meshes_.Get(_ID);
    VertexBufferDatas_[Mesh->VertexBuffer.GetID()].AddRef();
    IndexBufferDatas_[Mesh->IndexBuffer.GetID()].AddRef();
    Mesh.AddRef();
  }

  void Release(vdl::ID _ID)override
  {
    ReferenceCount<Mesh>& Mesh = Meshes_.Get(_ID);
    VertexBufferDatas_[Mesh->VertexBuffer.GetID()].Release();
    IndexBufferDatas_[Mesh->IndexBuffer.GetID()].Release();
    Mesh.Release();
  }

  Mesh* GetMesh(vdl::ID _ID)override { return Meshes_.Get(_ID).Get(); }
};
