#include "ConstantBufferAllocater.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>

#include <assert.h>

void ConstantBufferAllocater::Initialize(vdl::uint _BufferSize)
{
  IBuffer* pConstantBuffer;
  Engine::Get<IDevice>()->CreateConstantBuffer(&pConstantBuffer, _BufferSize);
  pConstantBuffer_.reset(std::move(static_cast<IConstantBuffer*>(pConstantBuffer)));

  MemorySpaces_.emplace_back(0, _BufferSize);
}

vdl::uint ConstantBufferAllocater::Secure(vdl::uint _BufferSize)
{
  for (auto& MemorySpace : MemorySpaces_)
  {
    if (MemorySpace.BufferSize < _BufferSize)
    {
      continue;
    }

    const vdl::uint Offset = MemorySpace.Offset;

    const vdl::uint AllocateSize = (_BufferSize + 255) & ~255;
    MemorySpace.Offset += AllocateSize;
    MemorySpace.BufferSize -= AllocateSize;

    return Offset;
  }

  assert(false);
  return 0;
}

void ConstantBufferAllocater::Release(vdl::uint _Offset, vdl::uint _BufferSize)
{
  MemorySpaces_.emplace_back(_Offset, (_BufferSize + 255) & ~255);
  //  オフセット順にソート
  std::sort(MemorySpaces_.begin(), MemorySpaces_.end());

  for (auto Itr = MemorySpaces_.begin() + 1; Itr != MemorySpaces_.cend();)
  {
    auto PreviousItr = Itr - 1;

    if (PreviousItr->Offset + PreviousItr->BufferSize >= Itr->Offset)
    {
      PreviousItr->BufferSize += Itr->BufferSize;

      Itr = MemorySpaces_.erase(Itr);
    }
    else
    {
      ++Itr;
    }
  }
}
