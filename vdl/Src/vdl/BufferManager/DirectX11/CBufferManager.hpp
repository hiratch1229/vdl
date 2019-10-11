#pragma once
#include "../IBufferManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

#include <vdl/pch/DirectX11/pch.hpp>

class IDevice;

class CBufferManager : public IBufferManager
{
  ID3D11Device* pD3D11Device_;
private:
  IDevice* pDevice_;
  ReferenceCounts<IBuffer> Buffers_;
public:
  CBufferManager() = default;

  void Initialize()override;

  vdl::ID CreateConstantBuffer(vdl::uint _BufferSize)override;

  vdl::Detail::ConstantBufferData CloneConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer)override;

  void* GetBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer)override;

  vdl::uint GetBufferSize(const vdl::Detail::ConstantBufferData& _ConstantBuffer)override;

  vdl::ID CreateUnorderedAccessBuffer(vdl::uint _Stride, vdl::uint _BufferSize, void* _Buffer)override;

  void AddRef(const vdl::ID& _ID)override { Buffers_.Get(_ID).AddRef(); }

  void Release(const vdl::ID& _ID) override { Buffers_.Get(_ID).Release(); }

  IBuffer* GetBuffer(const vdl::ID& _ID)override { return Buffers_.Get(_ID).Get(); }
};
