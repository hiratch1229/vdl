#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Type3.hpp>
#include <vdl/Shader.hpp>
#include <vdl/Texture.hpp>
#include <vdl/Sampler.hpp>
#include <vdl/ConstantBuffer.hpp>
#include <vdl/UnorderedAccessBuffer.hpp>
#include <vdl/Variant.hpp>
#include <vdl/Flag.hpp>

#include <vector>

enum class ComputeCommandFlag : vdl::uint8_t
{
  eSetShader = 1 << 0,
  eSetShaderResource = 1 << 1,
  eSetSampler = 1 << 2,
  eSetConstantBuffer = 1 << 3,
  eSetUnorderedAccessObject = 1 << 4,
};
using ComputeCommandFlags = vdl::Flags<ComputeCommandFlag, vdl::uint8_t>;

class IBufferManager;

class ComputeCommandList
{
  using ComputeCommands = std::vector<ComputeCommandFlag>;
  using ShaderResources = std::vector<vdl::ShaderResource>;
  using Samplers = std::vector<vdl::Sampler>;
  using ConstantBuffers = std::vector<vdl::Detail::ConstantBufferData>;
  using UnorderedAccessObjects = std::vector<vdl::UnorderedAccessObject>;
private:
  static constexpr ComputeCommandFlags kSetShaderFlag = ComputeCommandFlag::eSetShader;
  static constexpr ComputeCommandFlags kSetShaderResourceFlag = ComputeCommandFlag::eSetShaderResource;
  static constexpr ComputeCommandFlags kSetSamplerFlag = ComputeCommandFlag::eSetSampler;
  static constexpr ComputeCommandFlags kSetConstantBufferFlag = ComputeCommandFlag::eSetConstantBuffer;
  static constexpr ComputeCommandFlags kSetUnorderedAccessObjectFlag = ComputeCommandFlag::eSetUnorderedAccessObject;
private:
  IBufferManager* pBufferManager_;
private:
  ComputeCommands ComputeCommands_;
  ComputeCommandFlags ComputeCommandFlags_;

  vdl::ComputeShader CurrentComputeShader_;
  ShaderResources CurrentShaderResources_;
  Samplers CurrentSamplers_;
  ConstantBuffers ConstantBuffers_;
  ConstantBuffers PreviousConstantBuffers_;
  ConstantBuffers CurrentConstantBuffers_;
  UnorderedAccessObjects CurrentUnorderedAccessObjects_;
  vdl::uint3 ThreadGroup_;
public:
  ComputeCommandList() = default;

  ~ComputeCommandList() = default;

  void Initialize();

  void Reset();

  [[nodiscard]] const ComputeCommands& GetComputeCommands()const { return ComputeCommands_; }

  [[nodiscard]] const vdl::ComputeShader& GetComputeShader()const { return CurrentComputeShader_; }

  [[nodiscard]] const ShaderResources& GetShaderResources()const { return CurrentShaderResources_; }

  [[nodiscard]] const Samplers& GetSamplers()const { return CurrentSamplers_; }

  [[nodiscard]] const ConstantBuffers& GetConstantBuffers()const { return ConstantBuffers_; }

  [[nodiscard]] const UnorderedAccessObjects& GetUnorderedAccessObjects()const { return CurrentUnorderedAccessObjects_; }

  [[nodiscard]] const vdl::uint3& GetThreadGroup()const { return ThreadGroup_; }

  void SetShader(const vdl::ComputeShader& _ComputeShader);

  void SetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[]);

  void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[]);

  void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[]);

  void SetUnorderedAccessObjects(vdl::uint _StartSlot, vdl::uint _UnorderedAccessObjectNum, const vdl::UnorderedAccessObject _UnorderedAccessObjects[]);

  void Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ);
};
