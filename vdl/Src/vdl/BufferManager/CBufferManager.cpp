#include "CBufferManager.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/Buffer/IBuffer.hpp>

#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/DetectMemoryLeak.hpp>

#include <assert.h>

namespace
{
  template<class Buffer>
  inline Buffer* Cast(IBuffer* _pBuffer)
  {
    static_assert(std::is_base_of<IBuffer, Buffer>::value);

    if constexpr (std::is_same<IConstantBuffer, Buffer>::value)
    {
      assert(_pBuffer->GetType() == BufferType::eConstantBuffer || _pBuffer->GetType() == BufferType::eCopyConstantBuffer);
    }

    return static_cast<Buffer*>(_pBuffer);
  }
}

void CBufferManager::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
}

vdl::ID CBufferManager::CreateVertexBuffer(vdl::uint _BufferSize)
{
  IBuffer* pBuffer;
  pDevice_->CreateVertexBuffer(&pBuffer, _BufferSize);

  return Buffers_.Add(pBuffer);
}

vdl::ID CBufferManager::CreateVertexBuffer(const void* _Vertices, vdl::uint _BufferSize)
{
  IBuffer* pBuffer;
  pDevice_->CreateVertexBuffer(&pBuffer, _Vertices, _BufferSize);

  return Buffers_.Add(pBuffer);
}

vdl::ID CBufferManager::CreateInstanceBuffer(vdl::uint _BufferSize)
{
  IBuffer* pBuffer;
  pDevice_->CreateInstanceBuffer(&pBuffer, _BufferSize);

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
  return pDevice_->CloneConstantBuffer(_ConstantBuffer);
}

void* CBufferManager::GetBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{
  IConstantBuffer* pConstantBuffer = Cast<IConstantBuffer>(GetBuffer(_ConstantBuffer.GetID()));
  return pConstantBuffer->GetBuffer();
}

vdl::uint CBufferManager::GetBufferSize(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{
  IConstantBuffer* pConstantBuffer = Cast<IConstantBuffer>(GetBuffer(_ConstantBuffer.GetID()));
  return pConstantBuffer->GetBufferSize();
}

vdl::ID CBufferManager::CreateUnorderedAccessBuffer(vdl::uint _Stride, vdl::uint _BufferSize, void* _Buffer)
{
  IBuffer* pBuffer;
  pDevice_->CreateUnorderedAccessBuffer(&pBuffer, _Stride, _BufferSize, _Buffer);

  return Buffers_.Add(pBuffer);
}
