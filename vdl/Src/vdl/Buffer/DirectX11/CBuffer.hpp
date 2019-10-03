#pragma once
#include "../IBuffer.hpp"

#include <vdl/Macro.hpp>

#include <d3d11.h>
#include <wrl.h>
#include <deque>

struct CVertexBuffer : public IBuffer
{
  Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
  vdl::uint Stride;
public:
  CVertexBuffer() = default;

  BufferType GetType()const override final { return BufferType::eVertexBuffer; }
};

struct CInstanceBuffer : public IBuffer
{
  Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
  vdl::uint Stride;
public:
  CInstanceBuffer() = default;

  BufferType GetType()const override final { return BufferType::eInstanceBuffer; }
};

struct CIndexBuffer : public IBuffer
{
  Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
  IndexType IndexType;
public:
  CIndexBuffer() = default;

  BufferType GetType()const override final { return BufferType::eIndexBuffer; }
};

struct CConstantBuffer : public IConstantBuffer
{
  Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
  void* Buffer;
  vdl::uint BufferSize;
public:
  CConstantBuffer() = default;

  CConstantBuffer(vdl::uint _BufferSize)
    : Buffer(new char[_BufferSize]), BufferSize(_BufferSize) {}

  ~CConstantBuffer() { vdl::Macro::SafeDelete(Buffer); }

  BufferType GetType()const override final { return BufferType::eConstantBuffer; }

  void* GetData() override final { return Buffer; }

  vdl::uint GetSize() override final { return BufferSize; }
};

struct CUnordererdAccessBuffer : public IBuffer
{
  Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
  void* Buffer;
  vdl::uint BufferSize;
public:
  CUnordererdAccessBuffer(vdl::uint _BufferSize)
    : Buffer(new char[_BufferSize]), BufferSize(_BufferSize) {}

  BufferType GetType()const override final { return BufferType::eUnorderedAccessBuffer; }
};
