#pragma once
#include <vdl/Types.hpp>

enum class BufferType : vdl::uint8_t
{
  eVertexBuffer,
  eInstanceBuffer,
  eIndexBuffer,
  eConstantBuffer,
  eCopyConstantBuffer,
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

  [[nodiscard]] virtual void* GetBuffer()const = 0;

  [[nodiscard]] virtual vdl::uint GetBufferSize()const = 0;
};
