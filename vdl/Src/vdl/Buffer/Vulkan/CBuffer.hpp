#pragma once
#include "../IBuffer.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

struct BufferData
{
  vk::UniqueBuffer Buffer;
  vk::UniqueDeviceMemory Memory;
  void* pData;
public:
  BufferData() = default;

  BufferData(BufferData &&_Data)noexcept
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
  vk::IndexType IndexType;
public:
  CIndexBuffer() = default;

  BufferType GetType()const final { return BufferType::eIndexBuffer; }
};

struct CConstantBuffer : public IConstantBuffer
{
  BufferData BufferData;
  vdl::uint BufferSize;
  vdl::uint Offset;
public:
  CConstantBuffer() = default;

  void* GetBuffer()const final { return BufferData.pData; }

  vdl::uint GetBufferSize()const final { return BufferSize; }

  BufferType GetType()const final { return BufferType::eConstantBuffer; }
};

struct CCopyConstantBuffer : public IConstantBuffer
{
  vk::Buffer ParentBuffer;
  vdl::uint BufferSize;
  vdl::uint Offset;
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
public:
  CUnordererdAccessBuffer() = default;

  BufferType GetType()const final { return BufferType::eUnorderedAccessBuffer; }
};
