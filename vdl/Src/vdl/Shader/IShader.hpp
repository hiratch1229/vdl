#pragma once
#include <vdl/RenderType.hpp>

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

struct IVertexShader : public IShader
{
  virtual vdl::RenderType GetRenderType()const = 0;
};
