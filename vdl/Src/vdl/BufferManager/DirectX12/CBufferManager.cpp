#include "CBufferManager.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/Vulkan/CDevice.hpp>

void CBufferManager::Initialize()
{
  CDevice* pDevice = static_cast<CDevice*>(Engine::Get<IDevice>());

  pDevice_ = pDevice;
}

vdl::ID CBufferManager::CreateVertexBuffer(vdl::uint _Stride, vdl::uint _BufferSize)
{
  IBuffer* pBuffer;
  pDevice_->CreateVertexBuffer(&pBuffer, _Stride, _BufferSize);

  return Buffers_.Add(pBuffer);
}

vdl::ID CBufferManager::CreateVertexBuffer(const void* _Vertices, vdl::uint _Stride, vdl::uint _BufferSize)
{
  IBuffer* pBuffer;
  pDevice_->CreateVertexBuffer(&pBuffer, _Vertices, _Stride, _BufferSize);

  return Buffers_.Add(pBuffer);
}

vdl::ID CBufferManager::CreateInstanceBuffer(vdl::uint _Stride, vdl::uint _BufferSize)
{
  IBuffer* pBuffer;
  pDevice_->CreateInstanceBuffer(&pBuffer, _Stride, _BufferSize);

  return Buffers_.Add(pBuffer);
}

vdl::ID CBufferManager::CreateIndexBuffer(vdl::uint _BufferSize, IndexType _IndexType)
{
  IBuffer* pBuffer;
  pDevice_->CreateIndexBuffer(&pBuffer, _BufferSize, _IndexType);

  return Buffers_.Add(pBuffer);
}

vdl::ID CBufferManager::CreateIndexBuffer(const void* _Indices, vdl::uint _BufferSize, IndexType _IndexType)
{
  IBuffer* pBuffer;
  pDevice_->CreateIndexBuffer(&pBuffer, _Indices, _BufferSize, _IndexType);

  return Buffers_.Add(pBuffer);
}

vdl::ID CBufferManager::CreateConstantBuffer(vdl::uint _BufferSize)
{
  IBuffer* pBuffer;
  pDevice_->CreateConstantBuffer(&pBuffer, _BufferSize);

  return Buffers_.Add(pBuffer);
}

vdl::Detail::ConstantBufferData CBufferManager::CloneConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{

}

void* CBufferManager::GetBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{

}

vdl::uint CBufferManager::GetBufferSize(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{

}

vdl::ID CBufferManager::CreateUnorderedAccessBuffer(vdl::uint _Stride, vdl::uint _BufferSize, void* _Buffer)
{
  IBuffer* pBuffer;
  pDevice_->CreateUnorderedAccessBuffer(&pBuffer, _Stride, _BufferSize, _Buffer);

  return Buffers_.Add(pBuffer);
}
