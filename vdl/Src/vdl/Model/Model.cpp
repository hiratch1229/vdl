#include <vdl/Model.hpp>

#include <vdl/Engine.hpp>
#include <vdl/ModelManager/IModelManager.hpp>

namespace vdl
{
  StaticMesh::StaticMesh(const StaticMeshData& _MeshData)
  {
    ID_ = Engine::Get<IModelManager>()->Load(_MeshData);
  }

  StaticMesh::StaticMesh(StaticMeshData&& _MeshData)
  {
    ID_ = Engine::Get<IModelManager>()->Load(std::move(_MeshData));
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
    return Engine::Get<IModelManager>()->GetMesh(ID_)->GlobalTransform;
  }

  //--------------------------------------------------

  SkinnedMesh::SkinnedMesh(const SkinnedMeshData& _MeshData)
  {
    ID_ = Engine::Get<IModelManager>()->Load(_MeshData);
  }

  SkinnedMesh::SkinnedMesh(SkinnedMeshData&& _MeshData)
  {
    ID_ = Engine::Get<IModelManager>()->Load(std::move(_MeshData));
  }

  SkinnedMesh::SkinnedMesh(const SkinnedMesh& _SkinnedMesh)
  {
    if (ID_ = _SkinnedMesh.ID_)
    {
      Engine::Get<IModelManager>()->AddRef(ID_);
    }
  }

  SkinnedMesh::SkinnedMesh(SkinnedMesh&& _SkinnedMesh)noexcept
  {
    ID_ = _SkinnedMesh.ID_;
    _SkinnedMesh.ID_ = std::nullopt;
  }

  SkinnedMesh& SkinnedMesh::operator=(const SkinnedMesh& _SkinnedMesh)
  {
    if (ID_ != _SkinnedMesh.ID_)
    {
      if (ID_)
      {
        Engine::Get<IModelManager>()->Release(ID_);
      }
      if (ID_ = _SkinnedMesh.ID_)
      {
        Engine::Get<IModelManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  SkinnedMesh& SkinnedMesh::operator=(SkinnedMesh&& _SkinnedMesh)noexcept
  {
    ID_ = _SkinnedMesh.ID_;
    _SkinnedMesh.ID_ = std::nullopt;

    return *this;
  }

  //--------------------------------------------------

  Model::Model(const char* _FilePath, bool _isSerialize)
  {
    Meshes_ = Engine::Get<IModelManager>()->Load(_FilePath, _isSerialize);
  }

  Model::Model(const std::vector<StaticMesh>& _Meshes)
  {
    const size_t MeshNum = _Meshes.size();
    Meshes_.resize(MeshNum);

    for (size_t MeshCount = 0; MeshCount < MeshNum; ++MeshCount)
    {
      //  内部データは同じため安全
      Meshes_[MeshCount] = static_cast<const SkinnedMesh&>(_Meshes[MeshCount]);
    }
  }

  Model::Model(const std::vector<SkinnedMesh>& _Meshes)
  {
    Meshes_ = _Meshes;
  }
}
