#include "CComputer.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>

void CComputer::Initialize()
{
  pDevice_ = Engine::Get<IDevice>();
  pDeviceContext_ = Engine::Get<IDeviceContext>();
  pBufferManager_ = Engine::Get<IBufferManager>();
}

void CComputer::SetShader(const vdl::ComputeShader& _ComputeShader)
{
  if (StateChangeFlags_.Has(ComputerCommandType::eSetShader))
  {
    CurrentShader_ = _ComputeShader;

    if (CurrentShader_ == PreviousShader_)
    {
      StateChangeFlags_.Cancel(ComputerCommandType::eSetShader);
    }
  }
  else
  {
    if (CurrentShader_ != _ComputeShader)
    {
      CurrentShader_ = _ComputeShader;
      StateChangeFlags_.Set(ComputerCommandType::eSetShader);
    }
  }
}

#define SetShaderState(ShaderState)\
if (const vdl::uint RequiredSize = _StartSlot + _##ShaderState##Num;\
Current##ShaderState##s_.size() < RequiredSize)\
{\
  Current##ShaderState##s_.resize(static_cast<size_t>(RequiredSize));\
  StateChangeFlags_.Set(ComputerCommandType::eSet##ShaderState);\
}\
\
if (StateChangeFlags_.Has(ComputerCommandType::eSet##ShaderState))\
{\
  for (vdl::uint ShaderState##Count = 0; ShaderState##Count < _##ShaderState##Num; ++ShaderState##Count)\
  {\
    Current##ShaderState##s_[_StartSlot + ShaderState##Count] = _##ShaderState##s[ShaderState##Count];\
  }\
  \
  const size_t Current##ShaderState##Num = Current##ShaderState##s_.size();\
  if (Current##ShaderState##Num != Previous##ShaderState##s_.size())\
  {\
    return;\
  }\
  \
  for (vdl::uint ShaderState##Count = 0; ShaderState##Count < static_cast<vdl::uint>(Current##ShaderState##Num); ++ShaderState##Count)\
  {\
    if (Current##ShaderState##s_[ShaderState##Count] != Previous##ShaderState##s_[ShaderState##Count])\
    {\
      return;\
    }\
  }\
  \
  StateChangeFlags_.Cancel(ComputerCommandType::eSet##ShaderState);\
}\
else\
{\
  for (vdl::uint ShaderState##Count = 0; ShaderState##Count < _##ShaderState##Num; ++ShaderState##Count)\
  {\
    auto& Current##ShaderState = Current##ShaderState##s_[_StartSlot + ShaderState##Count];\
    const auto& ShaderState = _##ShaderState##s[ShaderState##Count];\
    \
    if (Current##ShaderState != ShaderState)\
    {\
      Current##ShaderState = ShaderState;\
      StateChangeFlags_.Set(ComputerCommandType::eSet##ShaderState);\
    }\
  }\
}

void CComputer::SetTextures(vdl::uint _StartSlot, vdl::uint _TextureNum, const vdl::Texture _Textures[])
{
  SetShaderState(Texture)
}

void CComputer::SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetShaderState(Sampler)
}

void CComputer::SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetShaderState(ConstantBuffer)
}

#undef SetShaderState

void CComputer::Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)
{
  if (!StateChangeFlags_.isEmpty())
  {
    if (StateChangeFlags_.Has(ComputerCommandType::eSetShader))
    {
      PreviousShader_ = CurrentShader_;
      pDeviceContext_->CSSetShader(CurrentShader_);
    }

    if (StateChangeFlags_.Has(ComputerCommandType::eSetTexture))
    {
      PreviousTextures_ = CurrentTextures_;
      pDeviceContext_->CSSetTextures(0, static_cast<vdl::uint>(CurrentTextures_.size()), CurrentTextures_.data());
    }

    if (StateChangeFlags_.Has(ComputerCommandType::eSetSampler))
    {
      PreviousSamplers_ = CurrentSamplers_;
      pDeviceContext_->CSSetSamplers(0, static_cast<vdl::uint>(CurrentSamplers_.size()), CurrentSamplers_.data());
    }
  }

  //  SetConstantBuffer
  {
    ConstantBuffers TempConstantBuffer = PreviousConstantBuffers_;
    {
      const size_t CurrentConstantBufferNum = CurrentConstantBuffers_.size();
      const size_t PreviousConstantBufferNum = PreviousConstantBuffers_.size();

      if (PreviousConstantBufferDatas_.size() < CurrentConstantBufferNum)
      {
        TempConstantBuffer.resize(CurrentConstantBufferNum);
      }

      size_t ConstantBufferCount = 0;
      for (; ConstantBufferCount < PreviousConstantBufferNum; ++ConstantBufferCount)
      {
        const vdl::Detail::ConstantBufferData& CurrentConstantBuffer = CurrentConstantBuffers_[ConstantBufferCount];
        const vdl::Detail::ConstantBufferData& PreviousConstantBuffer = PreviousConstantBuffers_[ConstantBufferCount];
        const vdl::Detail::ConstantBufferData& PreviousConstantBufferData = PreviousConstantBufferDatas_[ConstantBufferCount];

        if (CurrentConstantBuffer != PreviousConstantBuffer
          || ::memcmp(CurrentConstantBuffer.GetData(), PreviousConstantBufferData.GetData(), CurrentConstantBuffer.GetSize()))
        {
          TempConstantBuffer[ConstantBufferCount] = pBufferManager_->CloneConstantBuffer(CurrentConstantBuffer);
          StateChangeFlags_.Set(ComputerCommandType::eSetConstantBuffer);
        }
      }
      for (; ConstantBufferCount < CurrentConstantBufferNum; ++ConstantBufferCount)
      {
        TempConstantBuffer[ConstantBufferCount] = pBufferManager_->CloneConstantBuffer(CurrentConstantBuffers_[ConstantBufferCount]);
        StateChangeFlags_.Set(ComputerCommandType::eSetConstantBuffer);
      }

      if (StateChangeFlags_.Has(ComputerCommandType::eSetConstantBuffer))
      {
        PreviousConstantBuffers_ = CurrentConstantBuffers_;
        PreviousConstantBufferDatas_ = std::move(TempConstantBuffer);

        pDeviceContext_->CSSetConstantBuffers(0, static_cast<vdl::uint>(PreviousConstantBufferDatas_.size()), PreviousConstantBufferDatas_.data());
      }
    }
  }

  StateChangeFlags_.Clear();

  pDeviceContext_->Dispatch(_ThreadGroupX, _ThreadGroupY, _ThreadGroupZ);
}
