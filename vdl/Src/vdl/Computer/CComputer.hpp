#pragma once
#include "IComputer.hpp"

#include <vdl/Shader.hpp>
#include <vdl/Texture.hpp>
#include <vdl/Sampler.hpp>
#include <vdl/ConstantBuffer.hpp>

#include <vdl/Constants/Constants.hpp>

#include <vector>

class IDevice;
class IDeviceContext;
class IBufferManager;

enum class ComputerCommandType : vdl::uint8_t
{
  eSetShader,
  eSetTexture,
  eSetSampler,
  eSetConstantBuffer,

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

  void Set(ComputerCommandType _Command) { Flags_ |= 1 << static_cast<vdl::uint>(_Command); }

  void Cancel(ComputerCommandType _Command) { Flags_ &= ~static_cast<vdl::uint>(_Command); }
};

class CComputer : public IComputer
{
  using Samplers = std::vector<vdl::Sampler>;
  using Textures = std::vector<vdl::Texture>;
  using ConstantBuffers = std::vector<vdl::Detail::ConstantBufferData>;
private:
  IDevice* pDevice_;
  IDeviceContext* pDeviceContext_;
  IBufferManager* pBufferManager_;
private:
  StateChangeFlags StateChangeFlags_;

  vdl::ComputeShader PreviousShader_;
  Textures PreviousTextures_;
  Samplers PreviousSamplers_;
  ConstantBuffers PreviousConstantBufferDatas_;
  ConstantBuffers PreviousConstantBuffers_;

  vdl::ComputeShader CurrentShader_;
  Textures CurrentTextures_;
  Samplers CurrentSamplers_;
  ConstantBuffers CurrentConstantBuffers_;
public:
  CComputer() = default;

  void Initialize()override;

  void SetShader(const vdl::ComputeShader& _ComputeShader)override;

  void SetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])override;

  void SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])override;

  void SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])override;

  void Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)override;
};
