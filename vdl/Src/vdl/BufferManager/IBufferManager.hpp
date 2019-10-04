#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/ConstantBuffer.hpp>

#include <vdl/Buffer/IBuffer.hpp>

class IBufferManager
{
public:
  static IBufferManager* Create();

  IBufferManager() = default;

  virtual ~IBufferManager() = default;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual vdl::ID CreateConstantBuffer(vdl::uint _BufferSize) = 0;

  [[nodiscard]] virtual vdl::Detail::ConstantBufferData CloneConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBufferData) = 0;

  virtual void AddRef(const vdl::ID& _ID) = 0;

  virtual void Release(const vdl::ID& _ID) = 0;

  [[nodiscard]] virtual IBuffer* GetBuffer(const vdl::ID& _ID) = 0;
};
