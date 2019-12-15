#include "CBuffer.hpp"

#include <vdl/Engine.hpp>
#include <vdl/ConstantBufferAllocater/ConstantBufferAllocater.hpp>

CCopyConstantBuffer::~CCopyConstantBuffer()
{
  if (Engine::isActive())
  {
    pConstantBufferAllocater->Release(Offset, BufferSize);
  }
}

void* CCopyConstantBuffer::GetBuffer()const
{
  return pConstantBufferAllocater->GetBuffer(Offset);
}
