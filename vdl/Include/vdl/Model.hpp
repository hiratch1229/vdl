#pragma once
#include "Fwd.hpp"

#include <vector>

namespace vdl
{
  class StaticMesh
  {
  protected:
    ID ID_;
  public:
    StaticMesh() = default;

    StaticMesh(const VertexStaticMeshs& _Vertices, const Indices& _Indices, const StaticMeshData& _MeshData);

    StaticMesh(const StaticModelData& _StaticModelData, vdl::uint _MeshIndex = 0);

    StaticMesh(const StaticMesh& _StaticMesh);

    StaticMesh(StaticMesh&& _StaticMesh)noexcept;

    ~StaticMesh();
  public:
    StaticMesh& operator=(const StaticMesh& _StaticMesh);

    StaticMesh& operator=(StaticMesh&& _StaticMesh)noexcept;

    [[nodiscard]] constexpr bool operator==(const StaticMesh& _StaticMesh)const noexcept { return ID_ == _StaticMesh.ID_; }

    [[nodiscard]] constexpr bool operator!=(const StaticMesh& _StaticMesh)const noexcept { return ID_ != _StaticMesh.ID_; }
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }

    [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }

    [[nodiscard]] Matrix GetGlobalTransform()const;
  };

  class SkinnedMesh : public StaticMesh
  {
  public:
    SkinnedMesh() = default;

    SkinnedMesh(const VertexSkinnedMeshs& _Vertices, const Indices& _Indices, const StaticMeshData& _MeshData);

    SkinnedMesh(const SkinnedModelData& _SkinnedModelData, vdl::uint _MeshIndex = 0);
  public:
    [[nodiscard]] bool hasAnimation()const;
  };

  class Model
  {
    std::vector<SkinnedMesh> Meshes_;
  public:
    Model() = default;

    //  ÉtÉ@ÉCÉãÇ©ÇÁçÏê¨
    Model(const char* _FilePath, bool _isSerialize = true);

    Model(const SkinnedModelData& _SkinnedModelData);

    Model(const std::vector<SkinnedMesh>& _Meshes);
  public:
    [[nodiscard]] bool isEmpty()const noexcept { return Meshes_.empty(); }

    [[nodiscard]] bool hasAnimation()const noexcept { return !isEmpty() && Meshes_[0].hasAnimation(); }

    [[nodiscard]] const std::vector<SkinnedMesh>& GetMeshes()const { return Meshes_; }
  };
}
