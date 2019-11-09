#include <vdl/Model.hpp>

#include <vdl/Engine.hpp>
#include <vdl/ModelManager/IModelManager.hpp>

namespace vdl
{
  StaticMesh::StaticMesh(const VertexStaticMeshs& _Vertices, const Indices& _Indices, const StaticMeshData& _MeshData)
  {
    ID_ = Engine::Get<IModelManager>()->Load(_Vertices, _Indices, _MeshData);
  }

  StaticMesh::StaticMesh(const StaticModelData& _StaticModelData, vdl::uint _MeshIndex)
  {
    ID_ = Engine::Get<IModelManager>()->Load(_StaticModelData.Vertices, _StaticModelData.Indices, _StaticModelData.MeshDatas[_MeshIndex]);
  }

  StaticMesh::StaticMesh(const StaticMesh& _StaticMesh)
  {
    if (ID_ = _StaticMesh.ID_)
    {
      Engine::Get<IModelManager>()->AddRef(ID_);
    }
  }

  StaticMesh::StaticMesh(StaticMesh&& _StaticMesh)noexcept
  {
    ID_ = _StaticMesh.ID_;
    _StaticMesh.ID_ = std::nullopt;
  }

  StaticMesh& StaticMesh::operator=(const StaticMesh& _StaticMesh)
  {
    if (ID_ != _StaticMesh.ID_)
    {
      if (ID_)
      {
        Engine::Get<IModelManager>()->Release(ID_);
      }
      if (ID_ = _StaticMesh.ID_)
      {
        Engine::Get<IModelManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  StaticMesh& StaticMesh::operator=(StaticMesh&& _StaticMesh)noexcept
  {
    if (ID_)
    {
      Engine::Get<IModelManager>()->Release(ID_);
    }

    ID_ = _StaticMesh.ID_;
    _StaticMesh.ID_ = std::nullopt;

    return *this;
  }

  StaticMesh::~StaticMesh()
  {
    if (ID_)
    {
      Engine::Get<IModelManager>()->Release(ID_);
    }
  }

  Matrix StaticMesh::GetGlobalTransform()const
  {
    assert(!isEmpty());

    return Engine::Get<IModelManager>()->GetMesh(ID_)->GlobalTransform;
  }

  //--------------------------------------------------

  SkinnedMesh::SkinnedMesh(const VertexSkinnedMeshs& _Vertices, const Indices& _Indices, const StaticMeshData& _MeshData)
  {
    ID_ = Engine::Get<IModelManager>()->Load(_Vertices, _Indices, _MeshData);
  }

  SkinnedMesh::SkinnedMesh(const SkinnedModelData& _SkinnedModelData, vdl::uint _MeshIndex)
  {
    ID_ = Engine::Get<IModelManager>()->Load(_SkinnedModelData.Vertices, _SkinnedModelData.Indices, _SkinnedModelData.MeshDatas[_MeshIndex]);
  }

  bool SkinnedMesh::hasAnimation()const
  {
    return !Engine::Get<IModelManager>()->GetMesh(ID_)->Animations.empty();
  }

  //--------------------------------------------------

  Model::Model(const char* _FilePath, bool _isSerialize)
  {
    Meshes_ = Engine::Get<IModelManager>()->Load(_FilePath, _isSerialize);
  }

  Model::Model(const SkinnedModelData& _SkinnedModelData)
  {
    const size_t MeshNum = _SkinnedModelData.MeshDatas.size();
    Meshes_.resize(MeshNum);
    for (size_t MeshCount = 0; MeshCount < MeshNum; ++MeshCount)
    {
      Meshes_[MeshCount] = SkinnedMesh(_SkinnedModelData.Vertices, _SkinnedModelData.Indices, _SkinnedModelData.MeshDatas[MeshCount]);
    }
  }

  Model::Model(const std::vector<SkinnedMesh>& _Meshes)
  {
    Meshes_ = _Meshes;
  }
}
