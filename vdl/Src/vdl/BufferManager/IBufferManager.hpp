#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/ConstantBuffer.hpp>
#include <vdl/UnorderedAccessBuffer.hpp>

#include <vdl/Platform/Platform.hpp>
#include <vdl/Buffer/Buffer.hpp>

#include <vdl/Buffer/IBuffer.hpp>

class IBufferManager
{
public:
  static IBufferManager* Create();

  IBufferManager() = default;

  virtual ~IBufferManager() = default;

  [[nodiscard]] virtual PlatformType GetPlatform()const = 0;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual vdl::ID CreateVertexBuffer(vdl::uint _BufferSize) = 0;

  [[nodiscard]] virtual vdl::ID CreateVertexBuffer(const void* _Vertices, vdl::uint _BufferSize) = 0;

  [[nodiscard]] virtual vdl::ID CreateInstanceBuffer(vdl::uint _BufferSize) = 0;

  [[nodiscard]] virtual vdl::ID CreateIndexBuffer(vdl::uint _BufferSize, IndexType _IndexType) = 0;

  [[nodiscard]] virtual vdl::ID CreateIndexBuffer(const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType) = 0;

  [[nodiscard]] virtual vdl::ID CreateConstantBuffer(vdl::uint _BufferSize) = 0;

  [[nodiscard]] virtual vdl::Detail::ConstantBufferData CloneConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer) = 0;

  [[nodiscard]] virtual void* GetBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer) = 0;

  [[nodiscard]] virtual vdl::uint GetBufferSize(const vdl::Detail::ConstantBufferData& _ConstantBuffer) = 0;

  [[nodiscard]] virtual vdl::ID CreateUnorderedAccessBuffer(vdl::uint _Stride, vdl::uint _BufferSize, void* _Buffer) = 0;

  virtual void AddRef(const vdl::ID& _ID) = 0;

  virtual void Release(const vdl::ID& _ID) = 0;

  [[nodiscard]] virtual IBuffer* GetBuffer(const vdl::ID& _ID) = 0;

  virtual void SetBuffer(const vdl::ID& _ID, IBuffer* _pBuffer) = 0;
};
