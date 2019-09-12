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

struct IBuffer
{
  IBuffer() = default;

  virtual ~IBuffer() = default;

  [[nodiscard]] virtual BufferType GetType()const = 0;

  [[nodiscard]] virtual void* GetData() = 0;
};
