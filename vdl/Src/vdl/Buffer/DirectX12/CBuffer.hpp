#pragma once
#include "../IBuffer.hpp"

#include <vdl/pch/DirectX12/pch.hpp>
#include <vdl/Platform/IPlatform.hpp>

#include <assert.h>
#include <utility>

class MemoryAllocator;

struct BufferData
{
  Microsoft::WRL::ComPtr<ID3D12Resource> pResource;
  void* pBuffer;
public:
  BufferData() = default;

  BufferData(BufferData&& _Data)noexcept
    : pResource(std::move(_Data.pResource)), pBuffer(std::move(_Data.pBuffer)) {}

  BufferData& operator=(BufferData&& _Data)noexcept
  {
    pResource = std::move(_Data.pResource);
    pBuffer = std::move(_Data.pBuffer);

    return *this;
  }
};

struct CVertexBuffer : public IBuffer
{
  BufferData BufferData;
  D3D12_VERTEX_BUFFER_VIEW View;
public:
  CVertexBuffer() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

  BufferType GetType()const final { return BufferType::eVertexBuffer; }
};

struct CInstanceBuffer : public IBuffer
{
  BufferData BufferData;
  vdl::uint BufferSize;
  vdl::uint PreviousOffset;
  vdl::uint Offset;
public:
  CInstanceBuffer() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

  BufferType GetType()const final { return BufferType::eInstanceBuffer; }
};

struct CIndexBuffer : public IBuffer
{
  BufferData BufferData;
  D3D12_INDEX_BUFFER_VIEW View;
public:
  CIndexBuffer() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

  BufferType GetType()const final { return BufferType::eIndexBuffer; }
};

struct CConstantBuffer : public IConstantBuffer
{
  BufferData BufferData;
  vdl::uint BufferSize;
public:
  CConstantBuffer() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

  void* GetBuffer()const final { return BufferData.pBuffer; }

  vdl::uint GetBufferSize()const final { return BufferSize; }

  BufferType GetType()const final { return BufferType::eConstantBuffer; }
};

struct CCopyConstantBuffer : public IConstantBuffer
{
  MemoryAllocator* pConstantBufferAllocator;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pConstantBufferViewHeap;
  void* pBuffer;
  vdl::uint BufferSize;
public:
  CCopyConstantBuffer() = default;

  ~CCopyConstantBuffer();

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

  void* GetBuffer()const final { return pBuffer; }

  vdl::uint GetBufferSize()const final { return BufferSize; }

  BufferType GetType()const final { return BufferType::eCopyConstantBuffer; }
};

struct CUnordererdAccessBuffer : public IBuffer
{
  BufferData BufferData;
  vdl::uint BufferSize;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pShaderResourceViewHeap;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pUnordererdAccessViewHeap;
  D3D12_RESOURCE_STATES ResourceState;
public:
  CUnordererdAccessBuffer() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eDirectX12; }

  BufferType GetType()const final { return BufferType::eUnorderedAccessBuffer; }
};

template<class Buffer>
inline Buffer* Cast(IBuffer* _pBuffer)
{
  static_assert(std::is_base_of<IBuffer, Buffer>::value);

  const BufferType Type = _pBuffer->GetType();

  if constexpr (std::is_same<IConstantBuffer, Buffer>::value)
  {
    assert(Type == BufferType::eConstantBuffer || Type == BufferType::eCopyConstantBuffer);
  }
  else if constexpr (std::is_same<CVertexBuffer, Buffer>::value)
  {
    assert(Type == BufferType::eVertexBuffer);
  }
  else if constexpr (std::is_same<CInstanceBuffer, Buffer>::value)
  {
    assert(Type == BufferType::eInstanceBuffer);
  }
  else if constexpr (std::is_same<CIndexBuffer, Buffer>::value)
  {
    assert(Type == BufferType::eIndexBuffer);
  }
  else if constexpr (std::is_same<CConstantBuffer, Buffer>::value)
  {
    assert(Type == BufferType::eConstantBuffer);
  }
  else if constexpr (std::is_same<CCopyConstantBuffer, Buffer>::value)
  {
    assert(Type == BufferType::eCopyConstantBuffer);
  }
  else if constexpr (std::is_same<CUnordererdAccessBuffer, Buffer>::value)
  {
    assert(Type == BufferType::eUnorderedAccessBuffer);
  }
  else
  {
    static_assert(false);
  }

  return Cast<Buffer, IBuffer>(_pBuffer);
}
