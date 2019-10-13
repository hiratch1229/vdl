#include <vdl/Model.hpp>

#include <vdl/Engine.hpp>
#include <vdl/ModelManager/IModelManager.hpp>

namespace vdl
{
  Mesh::Mesh(const MeshData& _MeshData)
  {
    ID_ = Engine::Get<IModelManager>()->Load(_MeshData);
  }

  Mesh::Mesh(MeshData&& _MeshData)
  {
    ID_ = Engine::Get<IModelManager>()->Load(std::move(_MeshData));
  }

  Mesh::Mesh(const Mesh& _Mesh)
  {
    if (ID_ = _Mesh.ID_)
    {
      Engine::Get<IModelManager>()->AddRef(ID_);
    }
  }

  Mesh::Mesh(Mesh&& _Mesh)noexcept
  {
    ID_ = _Mesh.ID_;
    _Mesh.ID_ = std::nullopt;
  }

  Mesh& Mesh::operator=(const Mesh& _Mesh)
  {
    if (ID_ != _Mesh.ID_)
    {
      if (ID_)
      {
        Engine::Get<IModelManager>()->Release(ID_);
      }
      if (ID_ = _Mesh.ID_)
      {
        Engine::Get<IModelManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  Mesh& Mesh::operator=(Mesh&& _Mesh)noexcept
  {
    if (ID_)
    {
      Engine::Get<IModelManager>()->Release(ID_);
    }

    ID_ = _Mesh.ID_;
    _Mesh.ID_ = std::nullopt;

    return *this;
  }

  Mesh::~Mesh()
  {
    if (ID_)
    {
      Engine::Get<IModelManager>()->Release(ID_);
    }
  }

  Matrix Mesh::GetGlobalTransform()const
  {
    assert(!isEmpty());

    return Engine::Get<IModelManager>()->GetMesh(ID_)->GlobalTransform;
  }

  //--------------------------------------------------

  Model::Model(const char* _FilePath, bool _isSerialize)
  {
    Meshes_ = Engine::Get<IModelManager>()->Load(_FilePath, _isSerialize);
  }

  Model::Model(const std::vector<Mesh>& _Meshes)
  {
    Meshes_ = _Meshes;
  }
}
