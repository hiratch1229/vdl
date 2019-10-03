#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Types.hpp>

class IComputer
{
public:
  static IComputer* Create();

  IComputer() = default;

  virtual ~IComputer() = default;

  virtual void Initialize() = 0;

  virtual void SetShader(const vdl::ComputeShader& _ComputeShader) = 0;

  virtual void SetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[]) = 0;

  virtual void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]) = 0;

  virtual void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _BufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]) = 0;

  virtual void Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ) = 0;
};
