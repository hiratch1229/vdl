#include "CBuffer.hpp"

#include <vdl/Engine.hpp>
#include <vdl/MemoryAllocator/MemoryAllocator.hpp>

CCopyConstantBuffer::~CCopyConstantBuffer()
{
  if (Engine::isActive())
  {
    pConstantBufferAllocator->Release(pBuffer);
  }
}
