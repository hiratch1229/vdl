#pragma once
#include "../IBuffer.hpp"

#include <vdl/pch/DirectX12/pch.hpp>

#include <assert.h>
#include <utility>

struct BufferData
{
  Microsoft::WRL::ComPtr<ID3D12Resource> pResource;
  void* pData;
public:
  BufferData() = default;

  BufferData(BufferData&& _Data)noexcept
    : pResource(std::move(_Data.pResource)), pData(std::move(_Data.pData)) {}

  BufferData& operator=(BufferData&& _Data)noexcept
  {
    pResource = std::move(_Data.pResource);
    pData = std::move(_Data.pData);

    return *this;
  }
};

struct CVertexBuffer : public IBuffer
{
  BufferData BufferData;
  D3D12_VERTEX_BUFFER_VIEW View;
public:
  CVertexBuffer() = default;

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

  BufferType GetType()const final { return BufferType::eInstanceBuffer; }
};

struct CIndexBuffer : public IBuffer
{
  BufferData BufferData;
  D3D12_INDEX_BUFFER_VIEW View;
public:
  CIndexBuffer() = default;

  BufferType GetType()const final { return BufferType::eIndexBuffer; }
};

struct CConstantBuffer : public IConstantBuffer
{
  BufferData BufferData;
  vdl::uint BufferSize;
public:
  CConstantBuffer() = default;

  void* GetBuffer()const final { return BufferData.pData; }

  vdl::uint GetBufferSize()const final { return BufferSize; }

  BufferType GetType()const final { return BufferType::eConstantBuffer; }
};

struct CCopyConstantBuffer : public IConstantBuffer
{
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pConstantBufferViewHeap;
  vdl::uint Offset;
  vdl::uint BufferSize;
public:
  CCopyConstantBuffer() = default;

  ~CCopyConstantBuffer();

  void* GetBuffer()const final;

  vdl::uint GetBufferSize()const final { return BufferSize; }

  BufferType GetType()const final { return BufferType::eCopyConstantBuffer; }
};

struct CUnordererdAccessBuffer : public IBuffer
{
  BufferData BufferData;
  vdl::uint BufferSize;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pShaderResourceViewHeap;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pUnordererdAccessViewHeap;
public:
  CUnordererdAccessBuffer() = default;

  BufferType GetType()const final { return BufferType::eUnorderedAccessBuffer; }
};

template<class Buffer>
inline Buffer* Cast(IBuffer* _pBuffer)
{
  static_assert(std::is_base_of<IBuffer, Buffer>::value);

  if constexpr (std::is_same<IConstantBuffer, Buffer>::value)
  {
    assert(_pBuffer->GetType() == BufferType::eConstantBuffer || _pBuffer->GetType() == BufferType::eCopyConstantBuffer);
  }
  if constexpr (std::is_same<CVertexBuffer, Buffer>::value)
  {
    assert(_pBuffer->GetType() == BufferType::eVertexBuffer);
  }
  if constexpr (std::is_same<CInstanceBuffer, Buffer>::value)
  {
    assert(_pBuffer->GetType() == BufferType::eInstanceBuffer);
  }
  if constexpr (std::is_same<CIndexBuffer, Buffer>::value)
  {
    assert(_pBuffer->GetType() == BufferType::eIndexBuffer);
  }
  if constexpr (std::is_same<CConstantBuffer, Buffer>::value)
  {
    assert(_pBuffer->GetType() == BufferType::eConstantBuffer);
  }
  if constexpr (std::is_same<CCopyConstantBuffer, Buffer>::value)
  {
    assert(_pBuffer->GetType() == BufferType::eCopyConstantBuffer);
  }
  if constexpr (std::is_same<CUnordererdAccessBuffer, Buffer>::value)
  {
    assert(_pBuffer->GetType() == BufferType::eUnorderedAccessBuffer);
  }

  return static_cast<Buffer*>(_pBuffer);
}
