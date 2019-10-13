#include "CBufferManager.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/Vulkan/CDevice.hpp>

void CBufferManager::Initialize()
{
  CDevice* pDevice = static_cast<CDevice*>(Engine::Get<IDevice>());

  pDevice_ = pDevice;
  VkDevice_ = pDevice->GetDevice();

  ConstantBufferAllocater_.Initialize(kParentConstantBufferSize);
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
  assert(GetBuffer(_ConstantBuffer.GetID())->GetType() == BufferType::eConstantBuffer);

  const CConstantBuffer* pSrcConstantBuffer = static_cast<CConstantBuffer*>(GetBuffer(_ConstantBuffer.GetID()));

  vdl::Detail::ConstantBufferData ConstantBuffer(pSrcConstantBuffer->BufferSize);
  {
    CCopyConstantBuffer* pCopyConstantBuffer = new CCopyConstantBuffer;
    {
      pCopyConstantBuffer->ParentBuffer = ConstantBufferAllocater_.GetVkBuffer();
      pCopyConstantBuffer->BufferSize = pSrcConstantBuffer->BufferSize;
      pCopyConstantBuffer->Offset = ConstantBufferAllocater_.Secure(pCopyConstantBuffer->BufferSize);
    }

    ::memcpy(ConstantBufferAllocater_.GetBuffer(pCopyConstantBuffer->Offset), pSrcConstantBuffer->BufferData.pData, pCopyConstantBuffer->BufferSize);

    Buffers_.Get(ConstantBuffer.GetID()) = pCopyConstantBuffer;
  }

  return ConstantBuffer;
}

void* CBufferManager::GetBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{
  IBuffer* pConstantBuffer = GetBuffer(_ConstantBuffer.GetID());
  assert(pConstantBuffer->GetType() == BufferType::eConstantBuffer || pConstantBuffer->GetType() == BufferType::eCopyConstantBuffer);

  if (pConstantBuffer->GetType() == BufferType::eConstantBuffer)
  {
    return static_cast<CConstantBuffer*>(pConstantBuffer)->GetBuffer();
  }
  else
  {
    return static_cast<CCopyConstantBuffer*>(pConstantBuffer)->GetBuffer();
  }
}

vdl::uint CBufferManager::GetBufferSize(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{
  assert(GetBuffer(_ConstantBuffer.GetID())->GetType() == BufferType::eConstantBuffer || GetBuffer(_ConstantBuffer.GetID())->GetType() == BufferType::eCopyConstantBuffer);
  return static_cast<CConstantBuffer*>(GetBuffer(_ConstantBuffer.GetID()))->GetBufferSize();
}

vdl::ID CBufferManager::CreateUnorderedAccessBuffer(vdl::uint _Stride, vdl::uint _BufferSize, void* _Buffer)
{
  IBuffer* pBuffer;
  pDevice_->CreateUnorderedAccessBuffer(&pBuffer, _Stride, _BufferSize, _Buffer);

  return Buffers_.Add(pBuffer);
}
