#pragma once
#include "../IShader.hpp"

#include <vdl/pch/DirectX11/pch.hpp>

struct CVertexShader : public IVertexShader
{
  Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
  vdl::InputLayoutType InputLayout;
public:
  CVertexShader() = default;

  PlatformType GetPlatform()const final { return PlatformType::eDirectX11; }

  ShaderType GetType()const final { return ShaderType::eVertexShader; }

  vdl::InputLayoutType GetInputLayout()const final { return InputLayout; }
};

struct CHullShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11HullShader> pHullShader;
public:
  CHullShader() = default;

  PlatformType GetPlatform()const final { return PlatformType::eDirectX11; }

  ShaderType GetType()const final { return ShaderType::eHullShader; }
};

struct CDomainShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11DomainShader> pDomainShader;
public:
  CDomainShader() = default;

  PlatformType GetPlatform()const final { return PlatformType::eDirectX11; }

  ShaderType GetType()const final { return ShaderType::eDomainShader; }
};

struct CGeometryShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
public:
  CGeometryShader() = default;

  PlatformType GetPlatform()const final { return PlatformType::eDirectX11; }

  ShaderType GetType()const final { return ShaderType::eGeometryShader; }
};

struct CPixelShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
public:
  CPixelShader() = default;

  PlatformType GetPlatform()const final { return PlatformType::eDirectX11; }

  ShaderType GetType()const final { return ShaderType::ePixelShader; }
};

struct CComputeShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11ComputeShader> pComputeShader;
public:
  CComputeShader() = default;

  PlatformType GetPlatform()const final { return PlatformType::eDirectX11; }

  ShaderType GetType()const final { return ShaderType::eComputeShader; }
};

template<class Shader>
inline Shader* Cast(IShader* _pShader)
{
  static_assert(std::is_base_of<IShader, Shader>::value);

  const ShaderType Type = _pShader->GetType();

  if constexpr (std::is_same<CVertexShader, Shader>::value)
  {
    assert(Type == ShaderType::eVertexShader);
  }
  if constexpr (std::is_same<CHullShader, Shader>::value)
  {
    assert(Type == ShaderType::eHullShader);
  }
  if constexpr (std::is_same<CDomainShader, Shader>::value)
  {
    assert(Type == ShaderType::eDomainShader);
  }
  if constexpr (std::is_same<CGeometryShader, Shader>::value)
  {
    assert(Type == ShaderType::eGeometryShader);
  }
  if constexpr (std::is_same<CPixelShader, Shader>::value)
  {
    assert(Type == ShaderType::ePixelShader);
  }
  if constexpr (std::is_same<CComputeShader, Shader>::value)
  {
    assert(Type == ShaderType::eComputeShader);
  }

  return Cast<Shader, IShader>(_pShader);
}
