#pragma once
#include "Fwd.hpp"
#include "Type3.hpp"
#include "Texture.hpp"
#include "ConstantBuffer.hpp"
#include "UnorderedAccessBuffer.hpp"

namespace vdl::Computer
{
  void SetShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResource[]);

  void SetSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Samplers[]);

  void SetConstantBuffers(uint _StartSlot, uint _ConstantBufferNum, const Detail::ConstantBufferData _ConstantBuffers[]);

  template<class T>
  inline void SetConstantBuffers(uint _StartSlot, uint _ConstantBufferNum, const ConstantBuffer<T> _ConstantBuffers[])
  {
    std::vector<Detail::ConstantBufferData> ConstantBuffers(_ConstantBufferNum);
    {
      for (uint ConstantBufferCount = 0; ConstantBufferCount < _ConstantBufferNum; ++ConstantBufferCount)
      {
        ConstantBuffers[ConstantBufferCount] = _ConstantBuffers[ConstantBufferCount].GetDetail();
      }
    }

    SetConstantBuffers(_StartSlot, _ConstantBufferNum, ConstantBuffers.data());
  }

  void SetUnorderedAccessObjects(uint _StartSlot, uint _UnorderedAccessObjectNum, const UnorderedAccessObject _UnorderedAccessObjects[]);

  inline void SetUnorderedAccessObjects(uint _StartSlot, uint _UnorderedAccessTextureNum, const UnorderedAccessTexture _UnorderedAccessTextures[])
  {
    std::vector<UnorderedAccessObject> UnorderedAccessObjects(_UnorderedAccessTextureNum);
    {
      for (uint UnorderedAccessTextureCount = 0; UnorderedAccessTextureCount < _UnorderedAccessTextureNum; ++UnorderedAccessTextureCount)
      {
        UnorderedAccessObjects[UnorderedAccessTextureCount] = _UnorderedAccessTextures[UnorderedAccessTextureCount];
      }
    }

    SetUnorderedAccessObjects(_StartSlot, _UnorderedAccessTextureNum, UnorderedAccessObjects.data());
  }

  inline void SetUnorderedAccessObjects(uint _StartSlot, uint _UnorderedAccessBufferDataNum, const Detail::UnorderedAccessBufferData _UnorderedAccessBufferDatas[])
  {
    std::vector<UnorderedAccessObject> UnorderedAccessObjects(_UnorderedAccessBufferDataNum);
    {
      for (uint UnorderedAccessBufferDataCount = 0; UnorderedAccessBufferDataCount < _UnorderedAccessBufferDataNum; ++UnorderedAccessBufferDataCount)
      {
        UnorderedAccessObjects[UnorderedAccessBufferDataCount] = _UnorderedAccessBufferDatas[UnorderedAccessBufferDataCount];
      }
    }

    SetUnorderedAccessObjects(_StartSlot, _UnorderedAccessBufferDataNum, UnorderedAccessObjects.data());
  }

  template<class T>
  inline void SetUnorderedAccessObjects(uint _StartSlot, uint _UnorderedAccessBufferNum, const UnorderedAccessBuffer<T> _UnorderedAccessBuffers[])
  {
    std::vector<UnorderedAccessObject> UnorderedAccessObjects(_UnorderedAccessBufferNum);
    {
      for (uint UnorderedAccessBufferCount = 0; UnorderedAccessBufferCount < _UnorderedAccessBufferNum; ++UnorderedAccessBufferCount)
      {
        UnorderedAccessObjects[UnorderedAccessBufferCount] = _UnorderedAccessBuffers[UnorderedAccessBufferCount].GetDetail();
      }
    }

    SetUnorderedAccessObjects(_StartSlot, _UnorderedAccessBufferNum, UnorderedAccessObjects.data());
  }

  void SetShader(const ComputeShader& _Shader);

  void Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ);

  inline void Dispatch(const uint3& _ThreadGroup)
  {
    Dispatch(_ThreadGroup.x, _ThreadGroup.y, _ThreadGroup.z);
  }
}
