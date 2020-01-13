#pragma once
#include <vdl/Types.hpp>
#include <vdl/InputLayout.hpp>

#include <vdl/Platform/Platform.hpp>

enum class ShaderType : vdl::uint8_t
{
  eVertexShader,
  eHullShader,
  eDomainShader,
  eGeometryShader,
  ePixelShader,

  eComputeShader,

  eGraphicsNum = ePixelShader + 1,
  
  eNum
};

struct IShader
{
  IShader() = default;

  virtual ~IShader() = default;

  [[nodiscard]] virtual PlatformFlags GetPlatform()const = 0;

  [[nodiscard]] virtual ShaderType GetType()const = 0;

  //[[nodiscard]] virtual const std::vector<vdl::uint>& GetUsedConstantBufferRegisters()const = 0;
};

struct IVertexShader : public IShader
{
  IVertexShader() = default;

  [[nodiscard]] virtual vdl::InputLayoutType GetInputLayout()const = 0;
};

static constexpr vdl::uint kGraphicsShaderStageNum = static_cast<vdl::uint>(ShaderType::eGraphicsNum);
static constexpr vdl::uint kShaderStageNum = static_cast<vdl::uint>(ShaderType::eNum);
