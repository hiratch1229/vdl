#pragma once
#include "../IShader.hpp"

#include <vdl/pch/Vulkan/pch.hpp>
#include <vdl/Platform/IPlatform.hpp>

struct ShaderData
{
  vk::UniqueShaderModule Module;
  const char* EntryPoint;
public:
  ShaderData() = default;

  ShaderData(ShaderData&& _Data)noexcept
    : Module(std::move(_Data.Module)), EntryPoint(std::move(_Data.EntryPoint)) {}

  ShaderData& operator=(ShaderData&& _Data)noexcept
  {
    Module = std::move(_Data.Module);
    EntryPoint = std::move(_Data.EntryPoint);

    return *this;
  }
};

struct CVertexShader : public IVertexShader
{
  ShaderData ShaderData;
  vdl::InputLayoutType InputLayout;
public:
  CVertexShader() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eVulkan; }

  ShaderType GetType()const final { return ShaderType::eVertexShader; }

  vdl::InputLayoutType GetInputLayout()const final { return InputLayout; }
};

struct CHullShader : public IShader
{
  ShaderData ShaderData;
public:
  CHullShader() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eVulkan; }

  ShaderType GetType()const final { return ShaderType::eHullShader; }
};

struct CDomainShader : public IShader
{
  ShaderData ShaderData;
public:
  CDomainShader() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eVulkan; }

  ShaderType GetType()const final { return ShaderType::eDomainShader; }
};

struct CGeometryShader : public IShader
{
  ShaderData ShaderData;
public:
  CGeometryShader() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eVulkan; }

  ShaderType GetType()const final { return ShaderType::eGeometryShader; }
};

struct CPixelShader : public IShader
{
  ShaderData ShaderData;
public:
  CPixelShader() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eVulkan; }

  ShaderType GetType()const final { return ShaderType::ePixelShader; }
};

struct CComputeShader : public IShader
{
  ShaderData ShaderData;
public:
  CComputeShader() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eVulkan; }

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
  else if constexpr (std::is_same<CHullShader, Shader>::value)
  {
    assert(Type == ShaderType::eHullShader);
  }
  else if constexpr (std::is_same<CDomainShader, Shader>::value)
  {
    assert(Type == ShaderType::eDomainShader);
  }
  else if constexpr (std::is_same<CGeometryShader, Shader>::value)
  {
    assert(Type == ShaderType::eGeometryShader);
  }
  else if constexpr (std::is_same<CPixelShader, Shader>::value)
  {
    assert(Type == ShaderType::ePixelShader);
  }
  else if constexpr (std::is_same<CComputeShader, Shader>::value)
  {
    assert(Type == ShaderType::eComputeShader);
  }

  return Cast<Shader, IShader>(_pShader);
}

enum class DescriptorType : vdl::uint8_t
{
  eTexture,
  eBuffer,
  eSampler,
  eConstantBuffer,
  eUnorderedAccessBuffer,
  eUnorderedAccessTexture,

  eNum,
  eGraphicsNum = eConstantBuffer + 1,
};
static constexpr vdl::uint kGraphicsDescriptorTypeNum = static_cast<vdl::uint>(DescriptorType::eGraphicsNum);
static constexpr vdl::uint kComputeDescriptorTypeNum = static_cast<vdl::uint>(DescriptorType::eNum);
static constexpr vdl::uint kGraphicsDescriptorLayoutNum = kGraphicsDescriptorTypeNum * kGraphicsShaderStageNum;

inline vdl::uint GetDescriptorLayoutOffset(ShaderType _Stage, DescriptorType _Type)
{
  if (_Stage == ShaderType::eComputeShader)
  {
    return kGraphicsDescriptorLayoutNum + static_cast<vdl::uint>(_Type);
  }
  else
  {
    assert(_Type != DescriptorType::eUnorderedAccessBuffer && _Type != DescriptorType::eUnorderedAccessTexture);
    return static_cast<vdl::uint>(_Stage) * kGraphicsDescriptorTypeNum + static_cast<vdl::uint>(_Type);
  }
}
