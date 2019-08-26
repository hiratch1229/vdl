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

    //  静的メッシュデータから作成
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

    //  静的メッシュデータから作成
    SkinnedMesh(const StaticMeshData& _MeshData);

    //  スキンメッシュデータから作成
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

    //  ファイルから作成
    Model(const char* _FilePath);

    //  静的メッシュの配列から作成
    Model(const std::vector<StaticMesh>& _Meshes);

    //  スキンメッシュの配列から作成
    Model(const std::vector<SkinnedMesh>& _Meshes);
  };
}
