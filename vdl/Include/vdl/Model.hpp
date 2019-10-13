#pragma once
#include "Fwd.hpp"

#include <vector>

namespace vdl
{
  class Mesh
  {
  protected:
    ID ID_;
  public:
    Mesh() = default;

    //  静的メッシュデータから作成
    Mesh(const MeshData& _MeshData);

    Mesh(MeshData&& _MeshData);

    Mesh(const Mesh& _Mesh);

    Mesh(Mesh&& _Mesh)noexcept;
    
    ~Mesh();
  public:
    Mesh& operator=(const Mesh& _Mesh);

    Mesh& operator=(Mesh&& _Mesh)noexcept;

    [[nodiscard]] constexpr bool operator==(const Mesh& _Mesh)const noexcept { return ID_ == _Mesh.ID_; }

    [[nodiscard]] constexpr bool operator!=(const Mesh& _Mesh)const noexcept { return ID_ != _Mesh.ID_; }
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }

    [[nodiscard]] bool isEmpty()const noexcept { return ID_ == std::nullopt; }

    [[nodiscard]] Matrix GetGlobalTransform()const;
  };

  class Model
  {
    std::vector<Mesh> Meshes_;
  public:
    Model() = default;

    //  ファイルから作成
    Model(const char* _FilePath, bool _isSerialize = true);

    Model(const std::vector<Mesh>& _Meshes); 
  public:
    [[nodiscard]] bool isEmpty()const noexcept { return Meshes_.empty(); }

    [[nodiscard]] const std::vector<Mesh>& GetMeshes()const { return Meshes_; }
  };
}
