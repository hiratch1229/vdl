#pragma once
#include "../IBuffer.hpp"

#include <vdl/pch/DirectX11/pch.hpp>
#include <vdl/Platform/IPlatform.hpp>

#include <vdl/Macro.hpp>

#include <assert.h>

struct CVertexBuffer : public IBuffer
{
  Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
public:
  CVertexBuffer() = default;

  PlatformFlags GetPlatform()const override final { return PlatformFlag::eDirectX11; }

  BufferType GetType()const override final { return BufferType::eVertexBuffer; }
};

struct CInstanceBuffer : public IBuffer
{
  Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
public:
  CInstanceBuffer() = default;

  PlatformFlags GetPlatform()const override final { return PlatformFlag::eDirectX11; }

  BufferType GetType()const override final { return BufferType::eInstanceBuffer; }
};

struct CIndexBuffer : public IBuffer
{
  Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
  DXGI_FORMAT IndexFormat;
public:
  CIndexBuffer() = default;

  PlatformFlags GetPlatform()const override final { return PlatformFlag::eDirectX11; }

  BufferType GetType()const override final { return BufferType::eIndexBuffer; }
};

struct CConstantBuffer : public IConstantBuffer
{
  void* Buffer;
  vdl::uint BufferSize;
public:
  CConstantBuffer() = default;

  CConstantBuffer(vdl::uint _BufferSize)
    : Buffer(new char[_BufferSize]), BufferSize(_BufferSize) {}

  ~CConstantBuffer() { vdl::Macro::SafeDelete(Buffer); }

  PlatformFlags GetPlatform()const override final { return PlatformFlag::eDirectX11; }

  BufferType GetType()const override final { return BufferType::eConstantBuffer; }

  void* GetBuffer()const override final { return Buffer; }

  vdl::uint GetBufferSize()const override final { return BufferSize; }
};

struct CCopyConstantBuffer : public IConstantBuffer
{
  Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
  void* Buffer;
  vdl::uint BufferSize;
public:
  CCopyConstantBuffer() = default;

  CCopyConstantBuffer(vdl::uint _BufferSize)
    : Buffer(new char[_BufferSize]), BufferSize(_BufferSize) {}

  ~CCopyConstantBuffer() { vdl::Macro::SafeDelete(Buffer); }

  PlatformFlags GetPlatform()const override final { return PlatformFlag::eDirectX11; }

  void* GetBuffer()const final { return Buffer; }

  vdl::uint GetBufferSize()const final { return BufferSize; }

  BufferType GetType()const final { return BufferType::eCopyConstantBuffer; }
};

struct CUnordererdAccessBuffer : public IBuffer
{
  Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
  Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pUnorderedAccessView;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
  vdl::uint Stride;
  vdl::uint BufferSize;
public:
  CUnordererdAccessBuffer(vdl::uint _Stride, vdl::uint _BufferSize)
    : Stride(_Stride), BufferSize(_BufferSize) {}

  PlatformFlags GetPlatform()const override final { return PlatformFlag::eDirectX11; }

  BufferType GetType()const override final { return BufferType::eUnorderedAccessBuffer; }
};

template<class Buffer>
inline Buffer* Cast(IBuffer* _pBuffer)
{
  static_assert(std::is_base_of<IBuffer, Buffer>::value);

  const BufferType Type = _pBuffer->GetType();

  if constexpr (std::is_same<CVertexBuffer, Buffer>::value)
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
