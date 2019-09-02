#pragma once
#include <vdl/ID.hpp>

#include <vdl/Buffer/IBuffer.hpp>

class IBufferManager
{
public:
  static IBufferManager* Create();

  IBufferManager() = default;

  virtual ~IBufferManager() = default;

  virtual void Initialize() = 0;

  virtual ID CreateConstantBuffer(vdl::uint _BufferSize) = 0;

  virtual void AddRef(const ID& _ID) = 0;

  virtual void Release(const ID& _ID) = 0;

  virtual IBuffer* GetBuffer(const ID& _ID) = 0;
};
