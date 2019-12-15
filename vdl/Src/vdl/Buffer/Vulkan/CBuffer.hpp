#pragma once
#include "../IBuffer.hpp"

#include <vdl/pch/Vulkan/pch.hpp>

#include <assert.h>

class ConstantBufferAllocater;

struct BufferData
{
  vk::UniqueBuffer Buffer;
  vk::UniqueDeviceMemory Memory;
  void* pData;
public:
  BufferData() = default;

  BufferData(BufferData&& _Data)noexcept
    : Buffer(std::move(_Data.Buffer)), Memory(std::move(_Data.Memory)), pData(std::move(_Data.pData)) {}

  BufferData& operator=(BufferData&& _Data)noexcept
  {
    Buffer = std::move(_Data.Buffer);
    Memory = std::move(_Data.Memory);
    pData = std::move(_Data.pData);

    return *this;
  }
};

struct CVertexBuffer : public IBuffer
{
  BufferData BufferData;
public:
  CVertexBuffer() = default;

  PlatformType GetPlatform()const final { return PlatformType::eVulkan; }

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

  PlatformType GetPlatform()const final { return PlatformType::eVulkan; }

  BufferType GetType()const final { return BufferType::eInstanceBuffer; }
};

struct CIndexBuffer : public IBuffer
{
  BufferData BufferData;
  vk::IndexType IndexType;
public:
  CIndexBuffer() = default;

  PlatformType GetPlatform()const final { return PlatformType::eVulkan; }

  BufferType GetType()const final { return BufferType::eIndexBuffer; }
};

struct CConstantBuffer : public IConstantBuffer
{
  BufferData BufferData;
  vdl::uint BufferSize;
public:
  CConstantBuffer() = default;

  PlatformType GetPlatform()const final { return PlatformType::eVulkan; }

  void* GetBuffer()const final { return BufferData.pData; }

  vdl::uint GetBufferSize()const final { return BufferSize; }

  BufferType GetType()const final { return BufferType::eConstantBuffer; }
};

struct CCopyConstantBuffer : public IConstantBuffer
{
  ConstantBufferAllocater* pConstantBufferAllocater;
  vk::Buffer ParentBuffer;
  vdl::uint BufferSize;
  vdl::uint Offset;
public:
  CCopyConstantBuffer() = default;

  ~CCopyConstantBuffer();

  PlatformType GetPlatform()const final { return PlatformType::eVulkan; }

  void* GetBuffer()const final;

  vdl::uint GetBufferSize()const final { return BufferSize; }

  BufferType GetType()const final { return BufferType::eCopyConstantBuffer; }
};

struct CUnordererdAccessBuffer : public IBuffer
{
  BufferData BufferData;
  vdl::uint BufferSize;
public:
  CUnordererdAccessBuffer() = default;

  PlatformType GetPlatform()const final { return PlatformType::eVulkan; }

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
