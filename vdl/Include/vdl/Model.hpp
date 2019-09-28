#pragma once
#include "Fwd.hpp"
#include "ID.hpp"

#include <vector>

namespace vdl
{
  class StaticMesh
  {
  protected:
    ID ID_;
  public:
    StaticMesh() = default;

    //  静的メッシュデータから作成
    StaticMesh(const StaticMeshData& _MeshData);

    StaticMesh(StaticMeshData&& _MeshData);

    StaticMesh(const StaticMesh& _StaticMesh);

    StaticMesh(StaticMesh&& _StaticMesh)noexcept;
    
    ~StaticMesh();
  public:
    StaticMesh& operator=(const StaticMesh& _StaticMesh);

    StaticMesh& operator=(StaticMesh&& _StaticMesh)noexcept;

    [[nodiscard]] constexpr bool operator==(const StaticMesh& _Mesh)const noexcept { return ID_ == _Mesh.ID_; }

    [[nodiscard]] constexpr bool operator!=(const StaticMesh& _Mesh)const noexcept { return ID_ != _Mesh.ID_; }
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }

    [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }

    [[nodiscard]] Matrix GetGlobalTransform()const;
  };

  class SkinnedMesh : public StaticMesh
  {
  public:
    SkinnedMesh() = default;

    //  スキンメッシュデータから作成
    SkinnedMesh(const SkinnedMeshData& _MeshData);

    SkinnedMesh(SkinnedMeshData&& _MeshData);

    SkinnedMesh(const SkinnedMesh& _SkinnedMesh);

    SkinnedMesh(SkinnedMesh&& _SkinnedMesh)noexcept;
  public:
    SkinnedMesh& operator=(const SkinnedMesh& _SkinnedMesh);

    SkinnedMesh& operator=(SkinnedMesh&& _SkinnedMesh)noexcept;

    [[nodiscard]] constexpr bool operator==(const SkinnedMesh& _Mesh)const noexcept { return ID_ == _Mesh.ID_; }

    [[nodiscard]] constexpr bool operator!=(const SkinnedMesh& _Mesh)const noexcept { return ID_ != _Mesh.ID_; }
  };

  class Model
  {
    std::vector<SkinnedMesh> Meshes_;
  public:
    Model() = default;

    //  ファイルから作成
    Model(const char* _FilePath, bool _isSerialize = true);

    //  静的メッシュの配列から作成
    Model(const std::vector<StaticMesh>& _Meshes);

    //  スキンメッシュの配列から作成
    Model(const std::vector<SkinnedMesh>& _Meshes);
  public:
    [[nodiscard]] bool isEmpty()const noexcept { return Meshes_.empty(); }

    [[nodiscard]] const std::vector<SkinnedMesh>& GetMeshes()const { return Meshes_; }
  };
}
