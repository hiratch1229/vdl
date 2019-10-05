#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/ConstantBuffer.hpp>
#include <vdl/UnorderedAccessBuffer.hpp>

#include <vdl/Buffer/IBuffer.hpp>

class IBufferManager
{
public:
  static IBufferManager* Create();

  IBufferManager() = default;

  virtual ~IBufferManager() = default;

  virtual void Initialize() = 0;

  [[nodiscard]] virtual vdl::ID CreateConstantBuffer(vdl::uint _BufferSize) = 0;

  [[nodiscard]] virtual vdl::Detail::ConstantBufferData CloneConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer) = 0;

  [[nodiscard]] virtual void* GetBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer) = 0;

  [[nodiscard]] virtual vdl::uint GetBufferSize(const vdl::Detail::ConstantBufferData& _ConstantBuffer) = 0;

  [[nodiscard]] virtual vdl::ID CreateUnorderedAccessBuffer(vdl::uint _Stride, vdl::uint _BufferSize, void* _Buffer) = 0;

  virtual void AddRef(const vdl::ID& _ID) = 0;

  virtual void Release(const vdl::ID& _ID) = 0;

  [[nodiscard]] virtual IBuffer* GetBuffer(const vdl::ID& _ID) = 0;
};
