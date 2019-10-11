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
