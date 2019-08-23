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

  virtual BufferType GetType()const = 0;
};
