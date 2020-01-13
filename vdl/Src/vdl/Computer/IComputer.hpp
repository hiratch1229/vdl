#pragma once
#include <vdl/Fwd.hpp>

#include <vdl/Platform/Platform.hpp>

class IComputer
{
public:
  static IComputer* Create();

  IComputer() = default;

  virtual ~IComputer() = default;

  [[nodiscard]] virtual PlatformFlags GetPlatform()const = 0;

  virtual void Initialize() = 0;

  virtual void SetShader(const vdl::ComputeShader& _ComputeShader) = 0;

  virtual void SetShaderResource(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]) = 0;

  virtual void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]) = 0;

  virtual void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]) = 0;

  virtual void SetUnorderedAccessObjects(vdl::uint _StartSlot, vdl::uint _UnorderedAccessBufferNum, const vdl::UnorderedAccessObject _UnorderedAccessObjects[]) = 0;

  virtual void Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ) = 0;
};
