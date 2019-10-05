#include <vdl/Computer.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Computer/IComputer.hpp>

#include <vdl/Shader.hpp>

#include <assert.h>

namespace vdl::Computer
{
  void SetShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResource[])
  {
    Engine::Get<IComputer>()->SetShaderResource(_StartSlot, _ShaderResourceNum, _ShaderResource);
  }

  void SetSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Samplers[])
  {
    Engine::Get<IComputer>()->SetSamplers(_StartSlot, _SamplerNum, _Samplers);
  }

  void SetConstantBuffers(uint _StartSlot, uint _ConstantBufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
  {
    Engine::Get<IComputer>()->SetConstantBuffers(_StartSlot, _ConstantBufferNum, _ConstantBuffers);
  }

  void SetUnorderedAccessObjects(uint _StartSlot, uint _UnorderedAccessObjectNum, const UnorderedAccessObject _UnorderedAccessObjects[])
  {
    Engine::Get<IComputer>()->SetUnorderedAccessObjects(_StartSlot, _UnorderedAccessObjectNum, _UnorderedAccessObjects);
  }

  void SetShader(const ComputeShader& _Shader)
  {
    assert(!_Shader.isEmpty());

    Engine::Get<IComputer>()->SetShader(_Shader);
  }

  void Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)
  {
    Engine::Get<IComputer>()->Dispatch(_ThreadGroupX, _ThreadGroupY, _ThreadGroupZ);
  }
}
