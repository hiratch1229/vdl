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
    const ID& GetID()const { return ID_; }
  public:
    [[nodiscard]] constexpr bool operator==(const StaticMesh& _Mesh)const noexcept { return ID_ == _Mesh.ID_; }
    [[nodiscard]] constexpr bool operator!=(const StaticMesh& _Mesh)const noexcept { return ID_ != _Mesh.ID_; }
  public:
    StaticMesh() = default;

    ~StaticMesh() = default;

    //  �ÓI���b�V���f�[�^����쐬
    StaticMesh(const StaticMeshData& _MeshData);
  };

  class SkinnedMesh : public StaticMesh
  {
  public:
    [[nodiscard]] constexpr bool operator==(const SkinnedMesh& _Mesh)const noexcept { return ID_ == _Mesh.ID_; }
    [[nodiscard]] constexpr bool operator!=(const SkinnedMesh& _Mesh)const noexcept { return ID_ != _Mesh.ID_; }
  public:
    SkinnedMesh() = default;

    ~SkinnedMesh() = default;

    //  �ÓI���b�V���f�[�^����쐬
    SkinnedMesh(const StaticMeshData& _MeshData);

    //  �X�L�����b�V���f�[�^����쐬
    SkinnedMesh(const SkinnedMesh& _MeshData);
  };

  class Model
  {
    std::vector<SkinnedMesh> Meshes_;
  public:
    [[nodiscard]] bool isEmpty()const noexcept { return Meshes_.empty(); }
    [[nodiscard]] const std::vector<SkinnedMesh>& GetMeshes()const { return Meshes_; }
  public:
    Model() = default;

    ~Model() = default;

    //  �t�@�C������쐬
    Model(const char* _FilePath);

    //  �ÓI���b�V���̔z�񂩂�쐬
    Model(const std::vector<StaticMesh>& _Meshes);

    //  �X�L�����b�V���̔z�񂩂�쐬
    Model(const std::vector<SkinnedMesh>& _Meshes);
  };
}
