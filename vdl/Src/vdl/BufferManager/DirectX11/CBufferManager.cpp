#include "CBufferManager.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>

void CBufferManager::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
}

vdl::ID CBufferManager::CreateConstantBuffer(vdl::uint _BufferSize)
{
  IBuffer* pBuffer;
  pDevice_->CreateConstantBuffer(&pBuffer, _BufferSize);

  return Buffers_.Add(pBuffer);
}

vdl::Detail::ConstantBufferData CBufferManager::CloneConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBufferData)
{

}
