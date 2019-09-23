#pragma once
#include <vdl/Types.hpp>

enum class BufferType : vdl::uint8_t
{
  eVertexBuffer,
  eInstanceBuffer,
  eIndexBuffer,
  eConstantBuffer,
  eUnorderedAccessBuffer,
};

enum class IndexType : vdl::uint8_t
{
  eUint16,
  eUint32,
};

struct IBuffer
{
  IBuffer() = default;

  virtual ~IBuffer() = default;

  [[nodiscard]] virtual BufferType GetType()const = 0;
};

struct IConstantBuffer : public IBuffer
{
  IConstantBuffer() = default;

  [[nodiscard]] virtual void* GetData() = 0;

  [[nodiscard]] virtual vdl::uint GetSize() = 0;
};
