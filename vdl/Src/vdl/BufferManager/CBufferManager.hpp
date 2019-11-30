#pragma once
#include "IBufferManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

class IDevice;

class CBufferManager : public IBufferManager
{
  IDevice* pDevice_;
  ReferenceCounts<IBuffer> Buffers_;
public:
  CBufferManager() = default;

  void Initialize()override;

  vdl::ID CreateVertexBuffer(vdl::uint _BufferSize)override;

  vdl::ID CreateVertexBuffer(const void* _Vertices, vdl::uint _BufferSize)override;

  vdl::ID CreateInstanceBuffer(vdl::uint _BufferSize)override;

  vdl::ID CreateIndexBuffer(vdl::uint _BufferSize, IndexType _IndexType)override;

  vdl::ID CreateIndexBuffer(const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType)override;

  vdl::ID CreateConstantBuffer(vdl::uint _BufferSize)override;

  vdl::Detail::ConstantBufferData CloneConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer)override;

  void* GetBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer)override;

  vdl::uint GetBufferSize(const vdl::Detail::ConstantBufferData& _ConstantBuffer)override;

  vdl::ID CreateUnorderedAccessBuffer(vdl::uint _Stride, vdl::uint _BufferSize, void* _Buffer)override;

  void AddRef(const vdl::ID& _ID)override { Buffers_.Get(_ID).AddRef(); }

  void Release(const vdl::ID& _ID) override { Buffers_.Get(_ID).Release(); }

  IBuffer* GetBuffer(const vdl::ID& _ID)override { return Buffers_.Get(_ID).Get(); }

  void SetBuffer(const vdl::ID& _ID, IBuffer* _pBuffer)override { Buffers_.Get(_ID) = _pBuffer; }
};