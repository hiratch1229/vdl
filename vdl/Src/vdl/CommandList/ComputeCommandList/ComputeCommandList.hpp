#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Type3.hpp>
#include <vdl/Shader.hpp>
#include <vdl/Texture.hpp>
#include <vdl/Sampler.hpp>
#include <vdl/ConstantBuffer.hpp>
#include <vdl/UnorderedAccessBuffer.hpp>
#include <vdl/Variant.hpp>

#include <vector>

class IBufferManager;

class ComputeCommandList
{
  using ShaderResources = std::vector<vdl::ShaderResource>;
  using Samplers = std::vector<vdl::Sampler>;
  using ConstantBuffers = std::vector<vdl::Detail::ConstantBufferData>;
  using UnorderedAccessObjects = std::vector<vdl::UnorderedAccessObject>;
private:
  IBufferManager* pBufferManager_;
private:
  vdl::ComputeShader ComputeShader_;
  ShaderResources ShaderResources_;
  Samplers Samplers_;
  ConstantBuffers ConstantBuffers_;
  ConstantBuffers PreviousConstantBuffers_;
  ConstantBuffers CurrentConstantBuffers_;
  UnorderedAccessObjects UnorderedAccessObjects_;
  vdl::uint3 ThreadGroup_;
public:
  ComputeCommandList() = default;

  ~ComputeCommandList() = default;

  void Initialize();

  [[nodiscard]] const vdl::ComputeShader& GetComputeShader()const { return ComputeShader_; }

  [[nodiscard]] const ShaderResources& GetShaderResources()const { return ShaderResources_; }

  [[nodiscard]] const Samplers& GetSamplers()const { return Samplers_; }

  [[nodiscard]] const ConstantBuffers& GetConstantBuffers()const { return ConstantBuffers_; }

  [[nodiscard]] const UnorderedAccessObjects& GetUnorderedAccessObjects()const { return UnorderedAccessObjects_; }

  [[nodiscard]] const vdl::uint3& GetThreadGroup()const { return ThreadGroup_; }

  void SetShader(const vdl::ComputeShader& _ComputeShader)
  {
    ComputeShader_ = _ComputeShader;
  }

  void SetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
  {
    if (const vdl::uint RequiredSize = _StartSlot + _ShaderResourceNum;
      ShaderResources_.size() < RequiredSize)
    {
      ShaderResources_.resize(static_cast<size_t>(RequiredSize));
    }

    for (vdl::uint ShaderResourceCount = 0; ShaderResourceCount < _ShaderResourceNum; ++ShaderResourceCount)
    {
      ShaderResources_[_StartSlot + ShaderResourceCount] = _ShaderResources[ShaderResourceCount];
    }
  }

  void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
  {
    if (const vdl::uint RequiredSize = _StartSlot + _SamplerNum;
      Samplers_.size() < RequiredSize)
    {
      Samplers_.resize(static_cast<size_t>(RequiredSize));
    }

    for (vdl::uint SamplerCount = 0; SamplerCount < _SamplerNum; ++SamplerCount)
    {
      Samplers_[_StartSlot + SamplerCount] = _Samplers[SamplerCount];
    }
  }

  void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
  {
    if (const vdl::uint RequiredSize = _StartSlot + _ConstantBufferNum;
      CurrentConstantBuffers_.size() < RequiredSize)
    {
      CurrentConstantBuffers_.resize(static_cast<size_t>(RequiredSize));
    }

    for (vdl::uint ConstantBufferCount = 0; ConstantBufferCount < _ConstantBufferNum; ++ConstantBufferCount)
    {
      CurrentConstantBuffers_[_StartSlot + ConstantBufferCount] = _ConstantBuffers[ConstantBufferCount];
    }
  }

  void SetUnorderedAccessObjects(vdl::uint _StartSlot, vdl::uint _UnorderedAccessObjectNum, const vdl::UnorderedAccessObject _UnorderedAccessObjects[])
  {
    if (const vdl::uint RequiredSize = _StartSlot + _UnorderedAccessObjectNum;
      UnorderedAccessObjects_.size() < RequiredSize)
    {
      UnorderedAccessObjects_.resize(static_cast<size_t>(RequiredSize));
    }

    for (vdl::uint UnorderedAccessObjectCount = 0; UnorderedAccessObjectCount < _UnorderedAccessObjectNum; ++UnorderedAccessObjectCount)
    {
      UnorderedAccessObjects_[_StartSlot + UnorderedAccessObjectCount] = _UnorderedAccessObjects[UnorderedAccessObjectCount];
    }
  }

  void Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ);
};
