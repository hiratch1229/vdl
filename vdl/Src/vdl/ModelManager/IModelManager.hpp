#pragma once
#include <vdl/Fwd.hpp>

#include <vdl/Platform/Platform.hpp>

#include "Mesh.hpp"

class IModelManager
{
public:
  static IModelManager* Create();

  IModelManager() = default;

  virtual ~IModelManager() = default;

  [[nodiscard]] virtual PlatformFlags GetPlatform()const = 0;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual vdl::ID Load(const vdl::VertexStaticMeshs& _Vertices, const vdl::Indices& _Indices, const vdl::StaticMeshData& _StaticMeshData) = 0;

  [[nodiscard]] virtual vdl::ID Load(const vdl::VertexSkinnedMeshs& _Vertices, const vdl::Indices& _Indices, const vdl::SkinnedMeshData& _SkinnedMeshData) = 0;

  [[nodiscard]] virtual std::vector<vdl::SkinnedMesh> Load(const char* _FilePath) = 0;

  virtual void AddRef(vdl::ID _ID) = 0;

  virtual void Release(vdl::ID _ID) = 0;

  [[nodiscard]] virtual Mesh* GetMesh(vdl::ID _ID) = 0;
};
