#include "CBuffer.hpp"

#include <vdl/Engine.hpp>
#include <vdl/BufferManager/Vulkan/CBufferManager.hpp>

CCopyConstantBuffer::~CCopyConstantBuffer()
{
  static_cast<CBufferManager*>(Engine::Get<IBufferManager>())->GetConstantBufferAllocater()->Release(Offset, BufferSize);
}

void* CCopyConstantBuffer::GetBuffer()const
{
  return static_cast<CBufferManager*>(Engine::Get<IBufferManager>())->GetConstantBufferAllocater()->GetBuffer(Offset);
}
