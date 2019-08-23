#pragma once

enum class ShaderType
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
};
