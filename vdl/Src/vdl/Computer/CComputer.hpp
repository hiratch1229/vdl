#pragma once
#include "IComputer.hpp"
#include <vdl/CommandList/ComputeCommandList/ComputeCommandList.hpp>

class IDeviceContext;
class IRenderer;

class CComputer : public IComputer
{
  IDeviceContext* pDeviceContext_;
  IRenderer* pRenderer_;
private:
  ComputeCommandList ComputeCommandList_;
public:
  CComputer() = default;

  PlatformFlags GetPlatform()const final { return PlatformFlag::eCommon; }

  void Initialize()override;

  void SetShader(const vdl::ComputeShader& _ComputeShader)override;

  void SetShaderResource(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])override;

  void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void SetUnorderedAccessObjects(vdl::uint _StartSlot, vdl::uint _UnorderedAccessObjectNum, const vdl::UnorderedAccessObject _UnorderedAccessObjects[])override;

  void Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)override;
};
