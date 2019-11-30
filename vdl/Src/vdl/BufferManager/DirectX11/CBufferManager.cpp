#include "CBufferManager.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/DirectX11/CDevice.hpp>
#include <vdl/Buffer/DirectX11/CBuffer.hpp>

#include <vdl/Misc/Windows/Misc.hpp>

#include <vdl/DetectMemoryLeak.hpp>

#include <assert.h>

void CBufferManager::Initialize()
{
  CDevice* pDevice = static_cast<CDevice*>(Engine::Get<IDevice>());

  pDevice_ = pDevice;
  pD3D11Device_ = pDevice->GetDevice();
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
  const CConstantBuffer* pSrcConstantBuffer = Cast<CConstantBuffer>(GetBuffer(_ConstantBuffer.GetID()));

  vdl::Detail::ConstantBufferData ConstantBuffer(pSrcConstantBuffer->BufferSize);
  {
    CCopyConstantBuffer* pCopyConstantBuffer = new CCopyConstantBuffer(pSrcConstantBuffer->BufferSize);
    {
      ::memcpy(pCopyConstantBuffer->Buffer, pSrcConstantBuffer->Buffer, pCopyConstantBuffer->BufferSize);

      HRESULT hr = S_OK;

      D3D11_BUFFER_DESC BufferDesc;
      {
        BufferDesc.ByteWidth = pCopyConstantBuffer->BufferSize;
        BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        BufferDesc.CPUAccessFlags = 0;
        BufferDesc.MiscFlags = 0;
        BufferDesc.StructureByteStride = 0;
      }

      D3D11_SUBRESOURCE_DATA InitialData;
      {
        InitialData.pSysMem = pCopyConstantBuffer->Buffer;
        InitialData.SysMemPitch = 0;
        InitialData.SysMemSlicePitch = 0;
      }

      hr = pD3D11Device_->CreateBuffer(&BufferDesc, &InitialData, pCopyConstantBuffer->pBuffer.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
    }

    Buffers_.Get(ConstantBuffer.GetID()) = pCopyConstantBuffer;
  }

  return ConstantBuffer;
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
