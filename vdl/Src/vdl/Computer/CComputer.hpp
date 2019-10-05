#pragma once
#include "IComputer.hpp"

#include <vdl/Shader.hpp>
#include <vdl/Texture.hpp>
#include <vdl/Sampler.hpp>
#include <vdl/ConstantBuffer.hpp>
#include <vdl/UnorderedAccessBuffer.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vector>

class IDevice;
class IDeviceContext;
class IBufferManager;

enum class ComputerCommandType : vdl::uint8_t
{
  eSetShader,
  eSetShaderResource,
  eSetSampler,
  eSetConstantBuffer,
  eSetUnorderedAccessObject,

  eNum
};
static_assert(static_cast<vdl::uint>(ComputerCommandType::eNum) <= sizeof(vdl::uint8_t) * 8);

class StateChangeFlags
{
  vdl::uint8_t Flags_;
public:
  StateChangeFlags() = default;

  void Clear() { Flags_ = 0; }

  [[nodiscard]] bool isEmpty()const { return Flags_ == 0; }

  [[nodiscard]] bool Has(ComputerCommandType _Command)const { return Flags_ & (1 << static_cast<vdl::uint>(_Command)); }

  void Set(ComputerCommandType _Command) { Flags_ |= (1 << static_cast<vdl::uint>(_Command)); }

  void Cancel(ComputerCommandType _Command) { Flags_ &= ~(1 << static_cast<vdl::uint>(_Command)); }
};

class CComputer : public IComputer
{
  using Samplers = std::vector<vdl::Sampler>;
  using ShaderResources = std::vector<vdl::ShaderResource>;
  using ConstantBuffers = std::vector<vdl::Detail::ConstantBufferData>;
  using UnorderedAccessObjects = std::vector<vdl::UnorderedAccessObject>;
private:
  IDevice* pDevice_;
  IDeviceContext* pDeviceContext_;
  IBufferManager* pBufferManager_;
private:
  StateChangeFlags StateChangeFlags_;

  vdl::ComputeShader PreviousShader_;
  ShaderResources PreviousShaderResources_;
  Samplers PreviousSamplers_;
  ConstantBuffers PreviousConstantBuffers_;
  ConstantBuffers PreviousConstantBufferDatas_;
  UnorderedAccessObjects PreviousUnorderedAccessObjects_;

  vdl::ComputeShader CurrentShader_;
  ShaderResources CurrentShaderResources_;
  Samplers CurrentSamplers_;
  ConstantBuffers CurrentConstantBuffers_;
  UnorderedAccessObjects CurrentUnorderedAccessObjects_;
public:
  CComputer() = default;

  void Initialize()override;

  void SetShader(const vdl::ComputeShader& _ComputeShader)override;

  void SetShaderResource(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])override;

  void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void SetUnorderedAccessObjects(vdl::uint _StartSlot, vdl::uint _UnorderedAccessObjectNum, const vdl::UnorderedAccessObject _UnorderedAccessObjects[])override;

  void Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)override;
};
