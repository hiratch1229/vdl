#include <vdl/Model.hpp>

#include <vdl/Engine.hpp>
#include <vdl/ModelManager/IModelManager.hpp>

namespace vdl
{
  StaticMesh::StaticMesh(const StaticMeshData& _MeshData)
  {
    ID_ = Engine::Get<IModelManager>()->Load(_MeshData);
  }

  StaticMesh::StaticMesh(const StaticMesh& _StaticMesh)
  {
    if (ID_ = _StaticMesh.ID_)
    {
      Engine::Get<IModelManager>()->AddRef(ID_);
    }
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

  SkinnedMesh::SkinnedMesh(const SkinnedMesh& _SkinnedMesh)
  {
    if (ID_ = _SkinnedMesh.ID_)
    {
      Engine::Get<IModelManager>()->AddRef(ID_);
    }
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

  SkinnedMesh::~SkinnedMesh()
  {
    if (ID_)
    {
      Engine::Get<IModelManager>()->Release(ID_);
    }
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