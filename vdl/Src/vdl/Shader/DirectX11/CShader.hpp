#pragma once
#include "../IShader.hpp"

#include <d3d11.h>
#include <wrl.h>

struct CVertexShader : public IVertexShader
{
  Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
  vdl::InputLayout InputLayout;
public:
  CVertexShader() = default;

  ShaderType GetType()const override final { return ShaderType::eVertexShader; }

  vdl::InputLayout GetInputLayout()const override final { return InputLayout; }
};

struct CHullShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11HullShader> pHullShader;
public:
  CHullShader() = default;

  ShaderType GetType()const override final { return ShaderType::eHullShader; }
};

struct CDomainShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11DomainShader> pDomainShader;
public:
  CDomainShader() = default;

  ShaderType GetType()const override final { return ShaderType::eDomainShader; }
};

struct CGeometryShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
public:
  CGeometryShader() = default;

  ShaderType GetType()const override final { return ShaderType::eGeometryShader; }
};

struct CPixelShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
public:
  CPixelShader() = default;

  ShaderType GetType()const override final { return ShaderType::ePixelShader; }
};

struct CComputeShader : public IShader
{
  Microsoft::WRL::ComPtr<ID3D11ComputeShader> pComputeShader;
public:
  CComputeShader() = default;

  ShaderType GetType()const override final { return ShaderType::eComputeShader; }
};
