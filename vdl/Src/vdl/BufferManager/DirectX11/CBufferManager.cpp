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

vdl::Detail::ConstantBufferData CBufferManager::CloneConstantBuffer(const vdl::Detail::ConstantBufferData& _ConstantBufferData)
{
  const vdl::uint BufferSize = _ConstantBufferData.GetSize();
  vdl::Detail::ConstantBufferData ConstantBuffer(BufferSize);

  assert(Buffers_.Get(ConstantBuffer.GetID())->GetType() == BufferType::eConstantBuffer);

  HRESULT hr = S_OK;

  D3D11_BUFFER_DESC BufferDesc;
  {
    BufferDesc.ByteWidth = BufferSize;
    BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    BufferDesc.CPUAccessFlags = 0;
    BufferDesc.MiscFlags = 0;
    BufferDesc.StructureByteStride = 0;
  }

  D3D11_SUBRESOURCE_DATA InitialData;
  {
    InitialData.pSysMem = _ConstantBufferData.GetData();
    InitialData.SysMemPitch = 0;
    InitialData.SysMemSlicePitch = 0;
  }

  hr = pD3D11Device_->CreateBuffer(&BufferDesc, &InitialData, static_cast<CConstantBuffer*>(Buffers_.Get(ConstantBuffer.GetID()).Get())->pBuffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), hResultTrace(hr));
  
  return ConstantBuffer;
}
