#include "CBuffer.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/DirectX12/CDevice.hpp>

CCopyConstantBuffer::~CCopyConstantBuffer()
{
  static_cast<CDevice*>(Engine::Get<IDevice>())->GetConstantBufferAllocater()->Release(Offset, BufferSize);
}

void* CCopyConstantBuffer::GetBuffer()const
{
  return static_cast<CDevice*>(Engine::Get<IDevice>())->GetConstantBufferAllocater()->GetBuffer(Offset);
}
