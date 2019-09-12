#pragma once
#include "ID.hpp"
#include "MeshData.hpp"

namespace vdl
{
  class StaticMesh
  {
  protected:
    ID ID_;
  public:
    StaticMesh() = default;

    //  �ÓI���b�V���f�[�^����쐬
    StaticMesh(const StaticMeshData& _MeshData);

    StaticMesh(const StaticMesh& _StaticMesh);

    StaticMesh& operator=(const StaticMesh& _StaticMesh);

    ~StaticMesh();
  public:
    [[nodiscard]] constexpr bool operator==(const StaticMesh& _Mesh)const noexcept { return ID_ == _Mesh.ID_; }

    [[nodiscard]] constexpr bool operator!=(const StaticMesh& _Mesh)const noexcept { return ID_ != _Mesh.ID_; }
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }

    [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }
  };

  class SkinnedMesh : public StaticMesh
  {
  public:
    SkinnedMesh() = default;

    //  �X�L�����b�V���f�[�^����쐬
    SkinnedMesh(const SkinnedMeshData& _MeshData);

    SkinnedMesh(const SkinnedMesh& _SkinnedMesh);

    SkinnedMesh& operator=(const SkinnedMesh& _SkinnedMesh);

    ~SkinnedMesh();
  public:
    [[nodiscard]] constexpr bool operator==(const SkinnedMesh& _Mesh)const noexcept { return ID_ == _Mesh.ID_; }

    [[nodiscard]] constexpr bool operator!=(const SkinnedMesh& _Mesh)const noexcept { return ID_ != _Mesh.ID_; }
  };

  class Model
  {
    std::vector<SkinnedMesh> Meshes_;
  public:
    Model() = default;

    //  �t�@�C������쐬
    Model(const char* _FilePath, bool _isSerialize = true);

    //  �ÓI���b�V���̔z�񂩂�쐬
    Model(const std::vector<StaticMesh>& _Meshes);

    //  �X�L�����b�V���̔z�񂩂�쐬
    Model(const std::vector<SkinnedMesh>& _Meshes);

    ~Model() = default;
  public:
    [[nodiscard]] bool isEmpty()const noexcept { return Meshes_.empty(); }

    [[nodiscard]] const std::vector<SkinnedMesh>& GetMeshes()const { return Meshes_; }
  };
}
