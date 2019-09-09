#pragma once
#include <vdl/InputLayout.hpp>
#include <vdl/Types.hpp>

#include <vector>

enum class ShaderType : vdl::uint8_t
{
  eVertexShader,
  eHullShader,
  eDomainShader,
  eGeometryShader,
  ePixelShader,

  eComputeShader,
};

struct IShader
{
  IShader() = default;

  virtual ~IShader() = default;

  virtual ShaderType GetType()const = 0;

  virtual const std::vector<vdl::uint>& GetUsedConstantBufferRegisters()const = 0;
};

struct IVertexShader : public IShader
{
  virtual vdl::InputLayout GetInputLayout()const = 0;
};
