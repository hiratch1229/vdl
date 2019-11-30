#pragma once
#include "../IShader.hpp"

#include <vdl/pch/DirectX11/pch.hpp>

struct CVertexShader : public IVertexShader
{
  Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
  vdl::InputLayoutType InputLayout;
public:
  CVertexShader() = default;

  ShaderType GetType()const final { return ShaderType::eVertexShader; }

  vdl::InputLayoutType GetInputLayout()const final { return InputLayout; }
};

struct CHullShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11HullShader> pHullShader;
public:
  CHullShader() = default;

  ShaderType GetType()const final { return ShaderType::eHullShader; }
};

struct CDomainShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11DomainShader> pDomainShader;
public:
  CDomainShader() = default;

  ShaderType GetType()const final { return ShaderType::eDomainShader; }
};

struct CGeometryShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
public:
  CGeometryShader() = default;

  ShaderType GetType()const final { return ShaderType::eGeometryShader; }
};

struct CPixelShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
public:
  CPixelShader() = default;

  ShaderType GetType()const final { return ShaderType::ePixelShader; }
};

struct CComputeShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11ComputeShader> pComputeShader;
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
