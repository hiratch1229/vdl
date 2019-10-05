#include "CBufferManager.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/DirectX11/CDevice.hpp>
#include <vdl/Buffer/DirectX11/CBuffer.hpp>

#include <vdl/Misc/Windows/Misc.hpp>

#include <assert.h>

void CBufferManager::Initialize()
{
  CDevice* pDevice = static_cast<CDevice*>(Engine::Get<IDevice>());

  pDevice_ = pDevice;
  pD3D11Device_ = pDevice->GetDevice();
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
    CConstantBuffer* pDestConstantBuffer = static_cast<CConstantBuffer*>(GetBuffer(ConstantBuffer.GetID()));
    ::memcpy(pDestConstantBuffer->Buffer, pSrcConstantBuffer->Buffer, pDestConstantBuffer->BufferSize);

    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC BufferDesc;
    {
      BufferDesc.ByteWidth = pDestConstantBuffer->BufferSize;
      BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
      BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      BufferDesc.CPUAccessFlags = 0;
      BufferDesc.MiscFlags = 0;
      BufferDesc.StructureByteStride = 0;
    }

    D3D11_SUBRESOURCE_DATA InitialData;
    {
      InitialData.pSysMem = pDestConstantBuffer->Buffer;
      InitialData.SysMemPitch = 0;
      InitialData.SysMemSlicePitch = 0;
    }

    hr = pD3D11Device_->CreateBuffer(&BufferDesc, &InitialData, pDestConstantBuffer->pBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  }

  return ConstantBuffer;
}

void* CBufferManager::GetBuffer(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{
  assert(GetBuffer(_ConstantBuffer.GetID())->GetType() == BufferType::eConstantBuffer);
  return static_cast<CConstantBuffer*>(GetBuffer(_ConstantBuffer.GetID()))->GetBuffer();
}

vdl::uint CBufferManager::GetBufferSize(const vdl::Detail::ConstantBufferData& _ConstantBuffer)
{
  assert(GetBuffer(_ConstantBuffer.GetID())->GetType() == BufferType::eConstantBuffer);
  return static_cast<CConstantBuffer*>(GetBuffer(_ConstantBuffer.GetID()))->GetBufferSize();
}

vdl::ID CBufferManager::CreateUnorderedAccessBuffer(vdl::uint _Stride, vdl::uint _BufferSize, void* _Buffer)
{
  IBuffer* pBuffer;
  pDevice_->CreateUnorderedAccessBuffer(&pBuffer, _Stride, _BufferSize, _Buffer);

  return Buffers_.Add(pBuffer);
}
