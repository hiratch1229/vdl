#pragma once
#include "../IBufferManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

#include <d3d11.h>

class IDevice;

class CBufferManager : public IBufferManager
{
  IDevice* pDevice_;
private:
  ID3D11Device* pD3D11Device_;
  ReferenceCounts<IBuffer> Buffers_;
public:
  CBufferManager() = default;

  void Initialize()override;

  vdl::ID CreateConstantBuffer(vdl::uint _BufferSize)override;

  vdl::Detail::ConstantBufferData CloneConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBufferData)override;

  void AddRef(const vdl::ID& _ID)override { Buffers_.Get(_ID).AddRef(); }

  void Release(const vdl::ID& _ID) override { Buffers_.Get(_ID).Release(); }

  IBuffer* GetBuffer(const vdl::ID& _ID)override { return Buffers_.Get(_ID).Get(); }
};