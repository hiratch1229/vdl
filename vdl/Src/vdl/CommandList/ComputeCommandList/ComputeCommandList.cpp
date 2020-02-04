#include "ComputeCommandList.hpp"

#include <vdl/Engine.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>

#include <assert.h>

void ComputeCommandList::Initialize()
{
  pBufferManager_ = Engine::Get<IBufferManager>();
}

void ComputeCommandList::Reset()
{
  ComputeCommands_.clear();
}

void ComputeCommandList::SetShader(const vdl::ComputeShader& _ComputeShader)
{
  if (CurrentComputeShader_ == _ComputeShader)
  {
    return;
  }

  ComputeCommandFlags_ |= kSetShaderFlag;
  CurrentComputeShader_ = _ComputeShader;
}

#define SetShaderState(StateName)\
if (const vdl::uint RequiredSize = _StartSlot + _##StateName##Num;\
  Current##StateName##s_.size() < RequiredSize)\
{\
  Current##StateName##s_.resize(static_cast<size_t>(RequiredSize));\
}\
\
for (vdl::uint StateName##Count = 0; StateName##Count < _##StateName##Num; ++StateName##Count)\
{\
  auto& Current##StateName = Current##StateName##s_[_StartSlot + StateName##Count];\
  const auto& StateName = _##StateName##s[StateName##Count];\
  \
  if (Current##StateName == StateName)\
  {\
    continue;\
  }\
  \
  ComputeCommandFlags_ |= kSet##StateName##Flag;\
  Current##StateName = StateName;\
}

void ComputeCommandList::SetShaderResources(vdl::uint _StartSlot, vdl::uint _ShaderResourceNum, const vdl::ShaderResource _ShaderResources[])
{
  SetShaderState(ShaderResource)
}

void ComputeCommandList::SetSamplers(vdl::uint _StartSlot, vdl::uint _SamplerNum, const vdl::Sampler _Samplers[])
{
  SetShaderState(Sampler)
}

void ComputeCommandList::SetConstantBuffers(vdl::uint _StartSlot, vdl::uint _ConstantBufferNum, const vdl::Detail::ConstantBufferData _ConstantBuffers[])
{
  SetShaderState(ConstantBuffer)
}

void ComputeCommandList::SetUnorderedAccessObjects(vdl::uint _StartSlot, vdl::uint _UnorderedAccessObjectNum, const vdl::UnorderedAccessObject _UnorderedAccessObjects[])
{
  SetShaderState(UnorderedAccessObject)
}
#undef SetShaderState

void ComputeCommandList::Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)
{
  assert(!CurrentComputeShader_.isEmpty());

  ThreadGroup_ = { _ThreadGroupX, _ThreadGroupY, _ThreadGroupZ };

  //  定数バッファの更新
  {
    const vdl::uint CurrentConstantBufferNum = static_cast<vdl::uint>(CurrentConstantBuffers_.size());
    const vdl::uint PreviousConstantBufferNum = static_cast<vdl::uint>(PreviousConstantBuffers_.size());

    vdl::uint ConstantBufferCount;

    //  サイズを最新のものと同じにする
    if (PreviousConstantBufferNum < CurrentConstantBufferNum)
    {
      ConstantBuffers_.resize(static_cast<size_t>(CurrentConstantBufferNum));
      ComputeCommandFlags_ |= kSetConstantBufferFlag;

      //  サイズが増えた部分のデータのコピー
      for (ConstantBufferCount = PreviousConstantBufferNum; ConstantBufferCount < CurrentConstantBufferNum; ++ConstantBufferCount)
      {
        const vdl::Detail::ConstantBufferData& CurrentConstantBuffer = CurrentConstantBuffers_[ConstantBufferCount];

        if (CurrentConstantBuffer.isEmpty())
        {
          continue;
        }
        ConstantBuffers_[ConstantBufferCount] = pBufferManager_->CloneConstantBuffer(CurrentConstantBuffer);
      }
    }

    for (ConstantBufferCount = 0; ConstantBufferCount < PreviousConstantBufferNum; ++ConstantBufferCount)
    {
      const vdl::Detail::ConstantBufferData& CurrentConstantBuffer = CurrentConstantBuffers_[ConstantBufferCount];
      const vdl::Detail::ConstantBufferData& PreviousConstantBuffer = PreviousConstantBuffers_[ConstantBufferCount];
      vdl::Detail::ConstantBufferData& ConstantBuffer = ConstantBuffers_[ConstantBufferCount];

      //  データの更新が不要なら次へ
      if (CurrentConstantBuffer.isEmpty()
        || (CurrentConstantBuffer == PreviousConstantBuffer && ::memcmp(CurrentConstantBuffer.GetData(), ConstantBuffer.GetData(), CurrentConstantBuffer.GetBufferSize()) == 0))
      {
        continue;
      }

      ConstantBuffer = pBufferManager_->CloneConstantBuffer(CurrentConstantBuffer);
      ComputeCommandFlags_ |= kSetConstantBufferFlag;
    }

    PreviousConstantBuffers_ = CurrentConstantBuffers_;
  }

  if (ComputeCommandFlags_ & kSetShaderFlag)
  {
    ComputeCommands_.push_back(static_cast<ComputeCommandFlag>(static_cast<vdl::uint>(kSetShaderFlag)));
  }
  
  if (ComputeCommandFlags_ & kSetShaderResourceFlag)
  {
    ComputeCommands_.push_back(static_cast<ComputeCommandFlag>(static_cast<vdl::uint>(kSetShaderResourceFlag)));
  }
  
  if (ComputeCommandFlags_ & kSetSamplerFlag)
  {
    ComputeCommands_.push_back(static_cast<ComputeCommandFlag>(static_cast<vdl::uint>(kSetSamplerFlag)));
  }
  
  if (ComputeCommandFlags_ & kSetConstantBufferFlag)
  {
    ComputeCommands_.push_back(static_cast<ComputeCommandFlag>(static_cast<vdl::uint>(kSetConstantBufferFlag)));
  }
  
  if (ComputeCommandFlags_ & kSetUnorderedAccessObjectFlag)
  {
    ComputeCommands_.push_back(static_cast<ComputeCommandFlag>(static_cast<vdl::uint>(kSetUnorderedAccessObjectFlag)));
  }

  ComputeCommandFlags_ = 0;
}
