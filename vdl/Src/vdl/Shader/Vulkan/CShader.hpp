#pragma once
#include "../IShader.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

struct ShaderData
{
  vk::UniqueShaderModule Module;
  const char* EntryPoint;
public:
  ShaderData() = default;

  ShaderData(ShaderData &&_Data)noexcept
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

enum class DescriptorType : vdl::uint8_t
{
  eSampler,
  eShaderResouce,
  eConstantBuffer,
  eUnorderedAccessTexture,
  eUnorderedAccessBuffer,

  eNum,
  eGraphicsNum = eConstantBuffer +1,
};
static constexpr vdl::uint kGraphicsDescriptorTypeNum = static_cast<vdl::uint>(DescriptorType::eGraphicsNum);
static constexpr vdl::uint kComputeDescriptorTypeNum = static_cast<vdl::uint>(DescriptorType::eNum);
