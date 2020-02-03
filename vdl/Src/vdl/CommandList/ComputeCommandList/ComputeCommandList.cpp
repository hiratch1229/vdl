#include "ComputeCommandList.hpp"

#include <vdl/Engine.hpp>
#include <vdl/BufferManager/IBufferManager.hpp>

void ComputeCommandList::Initialize()
{
  pBufferManager_ = Engine::Get<IBufferManager>();
}

void ComputeCommandList::Dispatch(vdl::uint _ThreadGroupX, vdl::uint _ThreadGroupY, vdl::uint _ThreadGroupZ)
{
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
    }
 
    PreviousConstantBuffers_ = CurrentConstantBuffers_;
  }
}
