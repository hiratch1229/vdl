#pragma once
#include "Fwd.hpp"
#include "Resource.hpp"

#include <vector>

namespace vdl
{
  class StaticMesh : public Resource
  {
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
    [[nodiscard]] Matrix GetGlobalTransform()const;
  };

  class SkinnedMesh : public StaticMesh
  {
  public:
    SkinnedMesh() = default;

    SkinnedMesh(const VertexSkinnedMeshs& _Vertices, const Indices& _Indices, const SkinnedMeshData& _MeshData);

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
    Model(const char* _FilePath);

    Model(const StaticModelData& _StaticModelData);

    Model(const SkinnedModelData& _SkinnedModelData);

    Model(const std::vector<SkinnedMesh>& _Meshes);
  public:
    [[nodiscard]] SkinnedMesh& operator[](uint _Index) { return Meshes_[_Index]; }
  public:
    [[nodiscard]] bool isEmpty()const noexcept { return Meshes_.empty(); }

    [[nodiscard]] uint Size()const noexcept { return static_cast<uint>(Meshes_.size()); }

    [[nodiscard]] bool hasAnimation()const { return !isEmpty() && Meshes_[0].hasAnimation(); }

    [[nodiscard]] const std::vector<SkinnedMesh>& GetMeshes()const noexcept { return Meshes_; }
  };
}
