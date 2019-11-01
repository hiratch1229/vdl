#pragma once
#include "IModelManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

class IDevice;

class CModelManager : public IModelManager
{
  struct VertexBufferData
  {
    VertexBuffer VertexBuffer;
    vdl::Vertices Vertices;
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

  void Initialize()override {}

  vdl::ID Load(const vdl::Vertices& _Vertices, const vdl::Indices& _Indices, const vdl::MeshData& _MeshData)override;

  std::vector<vdl::Mesh> Load(const char* _FilePath, bool _isSerialize)override;

  void AddRef(const vdl::ID& _ID)override
  {
    ReferenceCount<Mesh>& Mesh = Meshes_.Get(_ID);
    VertexBufferDatas_[*Mesh->VertexBuffer.GetID()].AddRef();
    IndexBufferDatas_[*Mesh->IndexBuffer.GetID()].AddRef();
    Mesh.AddRef();
  }

  void Release(const vdl::ID& _ID)override
  {
    ReferenceCount<Mesh>& Mesh = Meshes_.Get(_ID);
    VertexBufferDatas_[*Mesh->VertexBuffer.GetID()].Release();
    IndexBufferDatas_[*Mesh->IndexBuffer.GetID()].Release();
    Mesh.Release();
  }

  Mesh* GetMesh(const vdl::ID& _ID)override { return Meshes_.Get(_ID).Get(); }
};
