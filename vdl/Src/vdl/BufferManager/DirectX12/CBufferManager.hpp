#pragma once
#include "../IBufferManager.hpp"

#include <vdl/ReferenceCount/ReferenceCount.hpp>

class IDevice;

class CBufferManager : public IBufferManager
{
  IDevice* pDevice_;
  ReferenceCounts<IBuffer> Buffers_;
public:
  CBufferManager() = default;

  void Initialize()override;

  ID CreateConstantBuffer(vdl::uint _BufferSize)override;

  void AddRef(const ID& _ID)override { Buffers_.Get(_ID).AddRef(); }

  void Release(const ID& _ID) override { Buffers_.Get(_ID).Release(); }
  
  IBuffer* GetBuffer(const ID& _ID)override { return Buffers_.Get(_ID).Get(); }
};
