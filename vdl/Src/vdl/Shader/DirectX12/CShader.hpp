#pragma once
#include "../IShader.hpp"

#include <vdl/pch/DirectX12/pch.hpp>

#include <assert.h>

struct ShaderData
{
  Microsoft::WRL::ComPtr<ID3D10Blob> pCode;
  D3D12_SHADER_BYTECODE ByteCode;
public:
  ShaderData() = default;

  ShaderData(ShaderData&& _Data)noexcept
    : pCode(std::move(_Data.pCode)), ByteCode(std::move(_Data.ByteCode)) {}

  ShaderData& operator=(ShaderData&& _Data)noexcept
  {
    pCode = std::move(_Data.pCode);
    ByteCode = std::move(_Data.ByteCode);

    return *this;
  }
};

struct CVertexShader : public IVertexShader
{
  ShaderData ShaderData;
  vdl::InputLayoutType InputLayout;
public:
  CVertexShader() = default;

  ShaderType GetType()const final { return ShaderType::eVertexShader; }

  vdl::InputLayoutType GetInputLayout()const final { return InputLayout; }
};

struct CHullShader : public IShader
{
  ShaderData ShaderData;
public:
  CHullShader() = default;

  ShaderType GetType()const final { return ShaderType::eHullShader; }
};

struct CDomainShader : public IShader
{
  ShaderData ShaderData;
public:
  CDomainShader() = default;

  ShaderType GetType()const final { return ShaderType::eDomainShader; }
};

struct CGeometryShader : public IShader
{
  ShaderData ShaderData;
public:
  CGeometryShader() = default;

  ShaderType GetType()const final { return ShaderType::eGeometryShader; }
};

struct CPixelShader : public IShader
{
  ShaderData ShaderData;
public:
  CPixelShader() = default;

  ShaderType GetType()const final { return ShaderType::ePixelShader; }
};

struct CComputeShader : public IShader
{
  ShaderData ShaderData;
public:
  CComputeShader() = default;

  ShaderType GetType()const final { return ShaderType::eComputeShader; }
};

template<class Shader>
inline Shader* Cast(IShader* _pShader)
{
  static_assert(std::is_base_of<IShader, Shader>::value);

  if constexpr (std::is_same<CVertexShader, Shader>::value)
  {
    assert(_pShader->GetType() == ShaderType::eVertexShader);
  }
  if constexpr (std::is_same<CHullShader, Shader>::value)
  {
    assert(_pShader->GetType() == ShaderType::eHullShader);
  }
  if constexpr (std::is_same<CDomainShader, Shader>::value)
  {
    assert(_pShader->GetType() == ShaderType::eDomainShader);
  }
  if constexpr (std::is_same<CGeometryShader, Shader>::value)
  {
    assert(_pShader->GetType() == ShaderType::eGeometryShader);
  }
  if constexpr (std::is_same<CPixelShader, Shader>::value)
  {
    assert(_pShader->GetType() == ShaderType::ePixelShader);
  }
  if constexpr (std::is_same<CComputeShader, Shader>::value)
  {
    assert(_pShader->GetType() == ShaderType::eComputeShader);
  }

  return static_cast<Shader*>(_pShader);
}
